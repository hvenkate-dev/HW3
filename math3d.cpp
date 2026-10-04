#include "math3d.h"

#include <cmath>

point_3d_t rotate_point_around_y(const point_3d_t &point, double angle_rads) {
    // y is the axis we spin around, so it stays the same; x and z swing in a circle.
    point_3d_t result;
    result.x = point.x * std::cos(angle_rads) - point.z * std::sin(angle_rads);
    result.y = point.y;
    result.z = point.x * std::sin(angle_rads) + point.z * std::cos(angle_rads);
    return result;
}

point_3d_t rotate_point_around_z(const point_3d_t &point, double angle_rads) {
    // z is the axis we spin around, so it stays the same; x and y swing in a circle.
    // Since z points at the viewer, this looks like turning a clock face on the screen.
    point_3d_t result;
    result.x = point.x * std::cos(angle_rads) - point.y * std::sin(angle_rads);
    result.y = point.x * std::sin(angle_rads) + point.y * std::cos(angle_rads);
    result.z = point.z;
    return result;
}

std::vector<line_3d_t> rotate_shape(const std::vector<line_3d_t> &shape,
                                    double angle_y, double angle_z) {
    // Order matters: y first (the pyramid spins on its own axis), then z (that spinning
    // pyramid is turned in the screen plane).
    std::vector<line_3d_t> rotated;
    for (const line_3d_t &line : shape) {
        point_3d_t a = rotate_point_around_z(rotate_point_around_y(line.a, angle_y), angle_z);
        point_3d_t b = rotate_point_around_z(rotate_point_around_y(line.b, angle_y), angle_z);
        rotated.push_back({a, b, line.color});
    }
    return rotated;
}

bool project_point(const point_3d_t &world_point, double camera_angle_rads,
                   point_2d_t &screen_point) {
    // Step 1: express the point relative to the camera, so the camera is the origin.
    point_3d_t relative = {world_point.x - CAMERA_POSITION.x,
                           world_point.y - CAMERA_POSITION.y,
                           world_point.z - CAMERA_POSITION.z};

    // Turning the camera is the same as spinning the whole world around the camera,
    // so we rotate the relative point around y.
    relative = rotate_point_around_y(relative, camera_angle_rads);

    // The camera looks down -z, so things in front of it have negative z.
    // Flip the sign so depth is a positive "distance in front of the camera".
    double depth = -relative.z;
    if (depth <= NEAR_PLANE) {
        return false;
    }

    // Step 2: perspective. Dividing by depth makes far things smaller
    // and pulls them toward the center of the screen.
    double projected_x = relative.x * FOCAL_LENGTH / depth;
    double projected_y = relative.y * FOCAL_LENGTH / depth;

    // Step 3: screen coordinates. The 3D origin is the middle of the view, but the
    // screen origin is the top-left corner, and screen y grows downward (3D y grows up).
    screen_point.x = static_cast<int>(SCREEN_WIDTH / 2.0 + projected_x);
    screen_point.y = static_cast<int>(SCREEN_HEIGHT / 2.0 - projected_y);
    return true;
}