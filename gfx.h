#ifndef GFX_H
#define GFX_H

#include "types.h"
#include <cstdint>

enum class input_event_t { NONE, LEFT, RIGHT, QUIT };

// Opens the window and allocates the frame buffer. Returns false on failure.
bool gfx_init();

// Fills the whole frame buffer with one 0x00RRGGBB color.
void gfx_clear(uint32_t color);

// Sets one pixel in the frame buffer. Pixels outside the screen are ignored.
void gfx_draw_pixel(int x, int y, uint32_t color);

// Draws a straight line between two screen points.
void gfx_draw_line(point_2d_t a, point_2d_t b, uint32_t color);

// Copies the frame buffer onto the window.
void gfx_present();

// Returns one pending input event, or NONE if the queue is empty.
input_event_t gfx_poll_event();

// Releases all X11 resources.
void gfx_close();

#endif