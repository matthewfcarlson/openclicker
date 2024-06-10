#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Creates WebGL context on DOM canvas element with ID "canvas". Sets CSS size and render target size to width&height.
void init_webgl(int width, int height);

// Establishes requestAnimationFrame() loop to be called on given callback function.
// t: absolute time, in msecs
// dt: delta time since previous frame, msecs.
void set_animation_frame_callback(void (*func)(double t, double dt));

// Returns random number in range 0...1
double rand01(void);

// WebGL canvas clear
void clear_screen(float r, float g, float b, float a);

// Fills a rectangle (x0,y0) -> (x1,y1) with given solid color rgba. Coordinates are in pixel units.
// Bottom-left corner of screen is (0,0). Top-right corner is (width-1,height-1) pixels.
void fill_solid_rectangle(float x0, float y0, float x1, float y1, float r, float g, float b, float a);

// Draws a string of text on screen at position (x0,y0). spacing: letter spacing. charSize: font size. shadow: if true, applies a drop shadow.
void fill_text(float x0, float y0, float r, float g, float b, float a, const char *str, float spacing, int charSize, int shadow);

// Draws an image from given url to pixel coordinates x0,y0, applying uniform scaling factor scale, modulated with rgba. 
void fill_image(float x0, float y0, float scale, float r, float g, float b, float a, const char *url);

// Draws an image from given tag to pixel coordinates x0,y0
void fill_image_tag(float x0, float y0, const char *tag);

// Loads and caches an image so we can draw it later.
void load_image_by_tag(const char *tag, const void* data, int w, int h);

// Plays the given audio file, looping infinitely if loop is nonzero.
void play_audio(const char *url, int loop);

#ifdef __cplusplus
}
#endif