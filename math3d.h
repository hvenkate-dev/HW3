#ifndef MATH3D_H
#define MATH3D_H

#include "types.h"
#include <vector>

// Rotates a point around the y axis (the vertical axis) by angle_rads.
point_3d_t rotate_point_around_y(const point_3d_t &point, double angle_rads);

// Rotates a point around the z axis (the axis pointing toward the camera) by angle_rads.
point_3d_t rotate_point_around_z(const point_3d_t &point, double angle_rads);

// Returns a copy of the shape rotated around y first, then around z.
std::vector<line_3d_t> rotate_shape(const std::vector<line_3d_t> &shape,
                                    double angle_y, double angle_z);

// Converts a 3D world point into a 2D screen pixel as seen by the (turned) camera.
// Returns false if the point is too close to / behind the camera to be drawn.
bool project_point(const point_3d_t &world_point, double camera_angle_rads,
                   point_2d_t &screen_point);

#endif