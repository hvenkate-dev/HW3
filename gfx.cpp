#include "gfx.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>   // <-- add this line (declares XDestroyImage)
#include <X11/keysym.h>

// File-local state: only this file talks to X11 directly.
static Display *display = nullptr;
static Window window;
static GC gc;
static XImage *image = nullptr;
static Atom wm_delete_window;
static std::vector<uint32_t> frame_buffer;

bool gfx_init() {
    display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        std::cerr << "Could not open X11 display! Exiting..." << std::endl;
        return false;
    }

    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0,
                                 SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                 BlackPixel(display, screen),
                                 BlackPixel(display, screen));
    XStoreName(display, window, "3D Viewer");
    XSelectInput(display, window, KeyPressMask);

    // Ask X11 to send us an event (instead of killing us) when the user clicks the close button.
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    XMapWindow(display, window);
    gc = DefaultGC(display, screen);

    // The image wraps our vector's memory, so drawing into frame_buffer is drawing into the image.
    frame_buffer.assign(SCREEN_WIDTH * SCREEN_HEIGHT, 0);
    image = XCreateImage(display, DefaultVisual(display, screen),
                         DefaultDepth(display, screen), ZPixmap, 0,
                         reinterpret_cast<char *>(frame_buffer.data()),
                         SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0);
    return true;
}

void gfx_clear(uint32_t color) {
    for (uint32_t &pixel : frame_buffer) {
        pixel = color;
    }
}

void gfx_draw_pixel(int x, int y, uint32_t color) {
    // Bounds check: writing outside the buffer would corrupt memory and can crash.
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    // The buffer is one long array; row y starts at y * SCREEN_WIDTH.
    frame_buffer[y * SCREEN_WIDTH + x] = color;
}

void gfx_draw_line(point_2d_t a, point_2d_t b, uint32_t color) {
    int dx = b.x - a.x;
    int dy = b.y - a.y;

    // Take as many steps as the longer side needs, so no gaps appear in the line.
    int steps = std::max(std::abs(dx), std::abs(dy));
    if (steps == 0) {
        gfx_draw_pixel(a.x, a.y, color);
        return;
    }

    for (int i = 0; i <= steps; i++) {
        double fraction = static_cast<double>(i) / steps;
        int x = a.x + static_cast<int>(std::round(dx * fraction));
        int y = a.y + static_cast<int>(std::round(dy * fraction));
        gfx_draw_pixel(x, y, color);
    }
}

void gfx_present() {
    XPutImage(display, window, gc, image, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    XFlush(display);
}

input_event_t gfx_poll_event() {
    while (XPending(display) > 0) {
        XEvent event;
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            if (key == XK_Left) return input_event_t::LEFT;
            if (key == XK_Right) return input_event_t::RIGHT;
            if (key == XK_Escape) return input_event_t::QUIT;
        } else if (event.type == ClientMessage &&
                   static_cast<Atom>(event.xclient.data.l[0]) == wm_delete_window) {
            return input_event_t::QUIT;
        }
    }
    return input_event_t::NONE;
}

void gfx_close() {
    if (image != nullptr) {
        // The vector owns this memory, so stop XDestroyImage from freeing it too.
        image->data = nullptr;
        XDestroyImage(image);
        image = nullptr;
    }
    if (display != nullptr) {
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        display = nullptr;
    }
}