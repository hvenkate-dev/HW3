#include "gfx.h"
#include "math3d.h"
#include "types.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

constexpr uint32_t COLOR_BACKGROUND = 0x00FFFFFF;  // white
constexpr uint32_t COLOR_AXIS = 0x00676767;
constexpr uint32_t COLOR_RED = 0x00FF0000;
constexpr uint32_t COLOR_GREEN = 0x0000FF00;
constexpr uint32_t COLOR_BLUE = 0x000000FF;

// Degrees added per frame, converted to radians. At 60 FPS that is 90 degrees per
// second around y (a full turn every 4 s) and 36 degrees per second around z (10 s).
constexpr double SPIN_Y_PER_FRAME = 1.5 * DEG_TO_RAD;
constexpr double SPIN_Z_PER_FRAME = 0.6 * DEG_TO_RAD;

constexpr double CAMERA_START_ANGLE = -0.1;   // radians
constexpr double CAMERA_TURN_PER_KEY = 0.05;  // radians

// One frame at 60 FPS lasts about 16.67 ms.
constexpr std::chrono::microseconds FRAME_DURATION(16667);

// Three grey axes crossing at the origin.
std::vector<line_3d_t> make_axes() {
    return {
        {{-50, 0, 0}, {50, 0, 0}, COLOR_AXIS},
        {{0, -50, 0}, {0, 50, 0}, COLOR_AXIS},
        {{0, 0, -50}, {0, 0, 50}, COLOR_AXIS},
    };
}

// Square-based pyramid. The origin sits halfway up its height, so the origin is
// always inside the pyramid no matter how it turns. Edge colors cycle red, green, blue.
std::vector<line_3d_t> make_pyramid() {
    point_3d_t apex = {0, 6, 0};
    point_3d_t base_a = {5, -6, 5};
    point_3d_t base_b = {-5, -6, 5};
    point_3d_t base_c = {-5, -6, -5};
    point_3d_t base_d = {5, -6, -5};

    return {
        {base_a, base_b, COLOR_RED},
        {base_b, base_c, COLOR_GREEN},
        {base_c, base_d, COLOR_BLUE},
        {base_d, base_a, COLOR_RED},
        {base_a, apex, COLOR_GREEN},
        {base_b, apex, COLOR_BLUE},
        {base_d, apex, COLOR_RED},
        {base_c, apex, COLOR_GREEN},
    };
}

// Projects every line and draws it, skipping lines with an endpoint behind the camera.
void draw_shape(const std::vector<line_3d_t> &shape, double camera_angle) {
    for (const line_3d_t &line : shape) {
        point_2d_t screen_a;
        point_2d_t screen_b;
        if (project_point(line.a, camera_angle, screen_a) &&
            project_point(line.b, camera_angle, screen_b)) {
            gfx_draw_line(screen_a, screen_b, line.color);
        }
    }
}

int main() {
    if (!gfx_init()) {
        return 1;
    }

    const std::vector<line_3d_t> axes = make_axes();
    const std::vector<line_3d_t> pyramid = make_pyramid();

    double angle_y = 0.0;
    double angle_z = 0.0;
    double camera_angle = CAMERA_START_ANGLE;
    int frame_count = 0;
    bool running = true;

    while (running) {
        auto frame_start = std::chrono::steady_clock::now();

        // Drain all queued events so keypresses don't pile up between frames.
        input_event_t event;
        while ((event = gfx_poll_event()) != input_event_t::NONE) {
            if (event == input_event_t::QUIT) {
                running = false;
            } else if (event == input_event_t::LEFT) {
                camera_angle -= CAMERA_TURN_PER_KEY;
            } else if (event == input_event_t::RIGHT) {
                camera_angle += CAMERA_TURN_PER_KEY;
            }
        }

        angle_y += SPIN_Y_PER_FRAME;
        angle_z += SPIN_Z_PER_FRAME;

        gfx_clear(COLOR_BACKGROUND);
        draw_shape(axes, camera_angle);
        // Always rotate the original pyramid by the total angles (not the previous
        // result) so rounding errors don't pile up over time.
        draw_shape(rotate_shape(pyramid, angle_y, angle_z), camera_angle);
        gfx_present();

        // Sleep for whatever is left of this frame's time budget.
        auto work_time = std::chrono::steady_clock::now() - frame_start;
        auto sleep_time = FRAME_DURATION - std::chrono::duration_cast<std::chrono::microseconds>(work_time);
        if (sleep_time.count() > 0) {
            std::this_thread::sleep_for(sleep_time);
        }

        // Printed once so you can answer the video question about sleep time.
        frame_count++;
        if (frame_count == 60) {
            std::cout << "Sleeping about " << sleep_time.count() / 1000.0
                      << " ms per frame" << std::endl;
        }
    }

    gfx_close();
    return 0;
}