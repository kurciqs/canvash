#include "shader.h"
#include "defines.h"

// NOTE i want a library that can render like processing. shouldn't be too hard :D
// NOTE runs on OpenGL and GLFW, the ONLY OBJECTIVELY CORRECT WAY TO DO THIS
// NOTE if going for camera stuff that might have to be implemented in the project not here in the library even though helper functions could be provided. or not it seems

// -------------------------------------------------------------

typedef enum {
    twodimensional,
    threedimensional
} CanvashMode;

// -------------------------------------------------------------

int canvash_init(const char *app_name, int app_width, int app_height, const char *app_icon_path, CanvashMode mode);

int canvash_running();

void canvash_clear_screen();

void canvash_background(vec3 background);

void canvash_render();

void canvash_terminate();

void canvash_set_key_callback(void (*key_callback)(int, int, int));

void canvash_set_mouse_callback(void (*mouse_callback)(int, int, float, float));

void canvash_rectangle_2D(vec2 p1, vec2 p2);

void canvash_ellipse_2D(vec2 p, float a, float b);

void canvash_rotate_2D(float rad);

void canvash_reset_transform_2D();

void canvash_scale_2D(float factorX, float factorY);

void canvash_transform_2D(float moveX, float moveY);

void canvash_fill_color(vec4 color);

void canvash_outline_color(vec4 color);

void canvash_no_fill();

void canvash_no_outline();

void canvash_outline_size(float size);

