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

void canvash_render();

void canvash_terminate();

// -------------------------------------------------------------

void canvash_set_key_callback(void (*key_callback)(int, int, int));

void canvash_set_mouse_callback(void (*mouse_callback)(int, int, float, float));

void canvash_get_window_size(float* width, float* height);

void canvash_get_mouse_position(float * x, float* y);

bool canvash_is_button_pressed(int button);

// -------------------------------------------------------------

void canvash_rectangle_2D(vec2 p1, vec2 p2);

void canvash_ellipse_2D(vec2 p, float a, float b);

void canvash_circle_2D(vec2 p, float r);

void canvash_triangle_2D(float *p1, float *p2, float *p3);

void canvash_line_2D(vec2 p1, vec2 p2);

// -------------------------------------------------------------

void canvash_rotate_2D(float rad);

void canvash_scale_2D(float factor_x, float factor_y);

void canvash_translate_2D(float move_x, float move_y);

void canvash_reset_transform_2D();

// -------------------------------------------------------------

void canvash_fill_color(vec4 color);

void canvash_no_fill();

void canvash_stroke_color(vec4 color);

void canvash_no_stroke();

void canvash_stroke(float strength);

void canvash_no_stroke();

// -------------------------------------------------------------

void canvash_background(vec3 background);

float canvash_time();

// -------------------------------------------------------------

bool canvash_is_key_down(int keycode);

bool canvash_is_mouse_down(int mousecode);