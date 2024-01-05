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
/*
typedef struct {
    GLFWwindow* window;
    void (*key_callback)(void*, int, int, int); // key_callback(void* canvash_instance, int key, int scancode, int action)
    void (*mouse_callback)(void*, int, int, float, float); // mouse_callback(void* canvash_instance, int button, int action, float xpos, float ypos)
    float* vertices;
    int*   indices;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint shader;
    int    num_draw_vertices;
    int    num_draw_indices;
    float* line_vertices;
    int*   line_indices;
    GLuint line_vao;
    GLuint line_vbo;
    GLuint line_ibo;
    GLuint line_shader;
    int    line_num_draw_vertices;
    int    line_num_draw_indices;
    CanvashMode mode;
    mat4 current_transform;
} CanvashInstance;
*/
// -------------------------------------------------------------

int canvash_init(const char* app_name, int app_width, int app_height, const char* app_icon_path, CanvashMode mode);

int canvash_running();

void canvash_clear_screen(vec3 background_color);

void canvash_render();

void canvash_terminate();

void canvash_set_key_callback(void (*key_callback)(int, int, int));

void canvash_set_mouse_callback(void (*mouse_callback)(int, int, float, float));

void canvash_rectangle_2D(const vec2 p1, const vec2 p2);

void canvash_rotate_2D(float rad);

void canvash_scale_2D(float factorX, float factorY);

void canvash_transform_2D(float moveX, float moveY);

