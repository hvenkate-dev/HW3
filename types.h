#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

// A point in the virtual 3D world.
struct point_3d_t {
    double x;
    double y;
    double z;
};

// A pixel position on the screen (origin at top-left, y grows downward).
struct point_2d_t {
    int x;
    int y;
};

// A colored line segment between two 3D points. Both the axes and the pyramid are made of these.
struct line_3d_t {
    point_3d_t a;
    point_3d_t b;
    uint32_t color;  // 0x00RRGGBB
};

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

// Bigger focal length means a more zoomed-in view.
constexpr double FOCAL_LENGTH = 600.0;

// Points at or behind this depth are skipped, because dividing by a tiny
// (or negative) depth would produce huge or mirrored coordinates.
constexpr double NEAR_PLANE = 0.0001;

// Camera sits here, looking straight down the -z axis.
constexpr point_3d_t CAMERA_POSITION = {5.0, 7.0, 70.0};

constexpr double PI = 3.14159265358979323846;
constexpr double DEG_TO_RAD = PI / 180.0;

#endif