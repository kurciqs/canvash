#include "canvash.h"

#define STB_IMAGE_IMPLEMENTATION

#include "lib/stb/include/stb_image.h"

// -------------------------------------------------------------

static GLFWwindow *s_window;
static vec2 s_window_size;

static void (*s_key_callback)(int, int, int); // key_callback(int key, int scancode, int action)
static void (*s_mouse_callback)(int, int, float, float); // mouse_callback(int button, int action, float xpos, float ypos)

static CanvashMode s_mode;

// NOTE have this be defined by like a CURRENT transform variable give by the user with rotate() etc and make it revertable
static mat4 s_current_transform;
static vec4 s_fill_color;
static bool s_fill;
// NOTE outlines are explicitly rendered with lines except in the case of ellipses
static vec4 s_outline_color;
static float s_outline_size;
static bool s_outline;
static vec3 s_background;
//static mat4 s_view; // 3D mode
static mat4 s_proj;

//--------------------------------------------------------------

// TODO think about how i could implement it so that measurements are in pixels
// TODO fill will not work with instance data it has to be added to the line rendering (separate)
// TODO gonna keep the color and the outline color and filling as static variables like the transform
// TODO fill without an outline_color is not getting drawn
// TODO safeguards for if not initialized
static int s_num_draw_rectangles = 0;
static float *s_rectangle_instance_data; /* mat4x4 transform (includes position given as argument), vec4 col (if w component is zero it is no_fill) */
static const size_t s_rectangle_instance_data_size = 4 * 4 * sizeof(float) + 4 * sizeof(float);
static const size_t s_rectangle_instance_data_num_floats = 4 * 4 + 4;
//static size_t s_rectangle_instance_data_size = 3 * sizeof(float);
static float s_rectangle_object_data_vertices[8] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
};
static unsigned int s_rectangle_object_data_indices[6] = {0, 1, 2, 0, 2, 3};
static Shader s_rectangle_shader;

// NOTE ellipse is also a circle
static int s_num_draw_ellipses;
static float *s_ellipse_instance_data; /* mat4x4 transform (includes position given as argument), vec4 col (if w component is zero it is no_fill), vec4 outline_color (if w component is zero it is no_outline), vec2 radius*/
static float s_ellipse_object_data_vertices[12] = {
        -0.5f, -0.5f, CANVASH_TWODIMENSIONAL_Z_POS,
        0.5f, -0.5f, CANVASH_TWODIMENSIONAL_Z_POS,
        0.5f, 0.5f, CANVASH_TWODIMENSIONAL_Z_POS,
        -0.5f, 0.5f, CANVASH_TWODIMENSIONAL_Z_POS
};
static int s_ellipse_object_data_indices[6] = {0, 1, 2, 0, 2, 3};
static Shader s_ellipse_shader;

// TODO change the instance data config
static int s_num_draw_triangles;
static float *s_triangles_object_data_vertices; /* mat4x4 transform, vec3 pos, vec4 col, bool fill */
static Shader s_triangle_shader;
// NOTE triangles have explicitly USER-DEFINED vertices so they are essentially batch rendered

// TODO this is really important for outlines
static int s_num_draw_lines;
static float *s_lines_object_data_vertices; /* mat4x4 transform, vec3 pos, vec4 col */
static Shader s_line_shader;
// NOTE lines have explicitly USER-DEFINED vertices so they are essentially batch rendered

// -------------------------------------------------------------

static void GLAPIENTRY opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    if (type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stderr, "[ERROR] opengl: %s type = 0x%x, severity = 0x%x, message = %s\n", "GL ERROR", type, severity, message);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr, "[ERROR] opengl: error code: 0x%8x\n", err);
        }
    }
}

void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "[ERROR] glfw: %d: %s\n", error, description);
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // NOTE: canvash built-in key callbacks
    if (CANVASH_KEY_IS_PRESSED(key, action, CANVASH_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == CANVASH_KEY_F11 && action == GLFW_PRESS) {
        // TODO enter/exit fullscreen
    }
    // NOTE: user defined key callback
    if (s_key_callback != NULL) {
        s_key_callback(key, scancode, action);
    }
}

void glfw_mouse_callback(GLFWwindow *window, int button, int action, int mods) {
    // NOTE: canvash built-in mouse callbacks
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // NOTE: user defined mouse callback
    if (s_mouse_callback != NULL) {
        s_mouse_callback(button, action, (float) xpos, (float) ypos);
    }
}

void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    s_window_size[0] = (float) width;
    s_window_size[1] = (float) height;
}

// -------------------------------------------------------------

int canvash_init(const char *app_name, int app_width, int app_height, const char *app_icon_path, CanvashMode mode) {
    s_mode = mode;
    if (s_mode == threedimensional) {
        fprintf(stderr, "[ERROR] threedimensional mode not working yet.\n");
        return 0;
    }

    if (!glfwInit()) {
        fprintf(stderr, "[ERROR] glfw failed to initiate.\n");
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(glfw_error_callback);

    s_window = glfwCreateWindow(app_width, app_height, app_name, NULL, NULL);
    if (!s_window) {
        glfwTerminate();
        fprintf(stderr, "[ERROR] glfw couldn't create window.\n");
        return 0;
    }

    glfwMakeContextCurrent(s_window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "[ERROR] glad failed to initialize.\n");
        return 0;
    }

    glViewport(0, 0, app_width, app_height);
    s_window_size[0] = (float) app_width;
    s_window_size[1] = (float) app_height;
    glfwSetFramebufferSizeCallback(s_window, glfw_framebuffer_size_callback);
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(opengl_message_callback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    } else {
        fprintf(stderr, "[INFO] opengl debug output not supported.\n");
    }
    glfwSwapInterval(1);

    {
        int width, height, channels;
        unsigned char *image = stbi_load(app_icon_path, &width, &height, &channels, 0);
        if (!image) {
            fprintf(stderr, "[ERROR] failed to load image %s.\n", app_icon_path);
            return 0;
        }

        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = image;
        glfwSetWindowIcon(s_window, 1, &icon);
    }

    glfwSetKeyCallback(s_window, glfw_key_callback);
    glfwSetMouseButtonCallback(s_window, glfw_mouse_callback);
    s_key_callback = NULL;
    s_mouse_callback = NULL;

    s_rectangle_shader = create_shader("res/shader/vert.glsl", "res/shader/frag.glsl");

    // NOTE set all of the styling data to basic (very gray and boring)
    glm_mat4_identity(s_current_transform);
    glm_mat4_identity(s_proj);
    glm_vec3_one(s_fill_color); // white
    glm_vec3_zero(s_outline_color); // black
    s_fill = true; // fill
    s_outline_size = 0.1f; // narrow
    s_outline = false; // outline
    s_background[0] = s_background[1] = s_background[2] = 0.5f; // gray

    return 1;
}

int canvash_running() {
    return glfwWindowShouldClose(s_window);
}

void canvash_clear_screen() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(s_background[0], s_background[1], s_background[2], 1.0f);
}

void canvash_render() {
    // TODO

    glm_ortho(-s_window_size[0] / 2.0f, s_window_size[0] / 2.0f, -s_window_size[1] / 2.0f, s_window_size[1] / 2.0f, -1.0f, 1.0f, s_proj);

    if (s_rectangle_instance_data && s_num_draw_rectangles) {

        float rectangle_object_data_vertices_new[12];

        int ind = 0;
        for (int i = 0; i < 4; i++) {
            rectangle_object_data_vertices_new[i * 3 + 0] = s_rectangle_object_data_vertices[ind++] * 400.0f;
            rectangle_object_data_vertices_new[i * 3 + 1] = s_rectangle_object_data_vertices[ind++] * 300.0f;
            rectangle_object_data_vertices_new[i * 3 + 2] = 0;
        }

        unsigned int vbo, ebo, vao, id_data_vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &id_data_vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_object_data_vertices_new), rectangle_object_data_vertices_new, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_rectangle_object_data_indices), s_rectangle_object_data_indices, GL_STATIC_DRAW);

        // TODO align the vertex data also in the shader
        // TODO projection and view matrices

        // NOTE this gives the position of the base quad vertices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
        glEnableVertexAttribArray(0);

        // NOTE this is different for each quad/rect
        glBindBuffer(GL_ARRAY_BUFFER, id_data_vbo);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) s_rectangle_instance_data_size * s_num_draw_rectangles, s_rectangle_instance_data, GL_STATIC_DRAW);

        // TODO get the model matrix to the GPU correctly

        // NOTE this is the model matrix
        glVertexAttribPointer(1, 16, GL_FLOAT, GL_FALSE, (GLsizei) (s_rectangle_instance_data_num_floats * sizeof(GLfloat)), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1); // This makes sure this attribute advances once per instance

        // NOTE this is the color
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_rectangle_instance_data_num_floats * sizeof(GLfloat)), (void *) (16 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1); // This also advances once per instance

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glUseProgram(s_rectangle_shader);
        glBindVertexArray(vao);

        shader_upload_m4(s_rectangle_shader, "u_proj", s_proj);

        // glDrawArraysInstanced(GL_TRIANGLES, 0, 4, numInstances);
        glDrawElementsInstanced(GL_TRIANGLES, sizeof(s_rectangle_object_data_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0, s_num_draw_rectangles);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &id_data_vbo);

        free((void *) s_rectangle_instance_data);
        s_rectangle_instance_data = NULL;
        s_num_draw_rectangles = 0;
    }

    glfwSwapBuffers(s_window);
    glfwPollEvents();

}

void canvash_terminate() {
    delete_shader(s_rectangle_shader);
    free((void *) s_rectangle_instance_data);

    glfwDestroyWindow(s_window);
    glfwTerminate();
}

void canvash_set_key_callback(void (*key_callback)(int, int, int)) {
    s_key_callback = key_callback;
}

void canvash_set_mouse_callback(void (*mouse_callback)(int, int, float, float)) {
    s_mouse_callback = mouse_callback;
}

// -------------------------------------------------------------

void canvash_rectangle_2D(vec2 p1, vec2 p2) {
    if (s_mode == threedimensional) {
        fprintf(stderr, "[ERROR] cannot call canvash_rectangle_2D while in threedimensional mode.\n");
        return;
    }

    // TODO this will render the outline with the line renderer
    if (s_outline) {
        fprintf(stderr, "[ERROR] cannot call canvash_rectangle_2D while in outline mode.\n");
        return;
    }

    if (s_fill) {
        // NOTE renders the fill mesh

        // NOTE resize the current instance data to fit with the added rectangle
        s_num_draw_rectangles++;
        if (!s_rectangle_instance_data && !s_num_draw_rectangles) {
            s_rectangle_instance_data = (float *) malloc(s_rectangle_instance_data_size * s_num_draw_rectangles);
            if (s_rectangle_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to allocate any rectangle instance data.\n");
                s_num_draw_rectangles--;
                return;
            }
        } else {
            float *new_rectangle_instance_data = (float *) realloc(s_rectangle_instance_data, s_rectangle_instance_data_size * s_num_draw_rectangles);
            if (new_rectangle_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to reallocate new rectangle instance data.\n");
                s_num_draw_rectangles--;
                return;
            } else {
                s_rectangle_instance_data = new_rectangle_instance_data;
            }
        }

        // NOTE add the instance data
        // NOTE 16 floats for transform matrix + 4 floats for color

        mat4 transform;
        glm_mat4_copy(s_current_transform, transform);

        vec2 center;
        glm_vec2_add(p1, p2, center);
        glm_vec2_scale(center, 0.5f, center);

//        glm_scale(transform, (vec3) {fabsf( p1[0] - p2[0]), fabsf( p1[1] - p2[1]), 1.0f }); // will scale by the width and height and since the initial size is unitary this will translate into pixels on the screen
//        glm_translate(transform, (vec3) {center[0], center[1], 0.0f});

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                s_rectangle_instance_data[(s_num_draw_rectangles - 1) * s_rectangle_instance_data_num_floats + i * 4 + j] = transform[i][j];
//                printf("%d %d %f %d\n", i, j, transform[i][j], (s_num_draw_rectangles - 1) * s_rectangle_instance_data_num_floats + i * 4 + j);
            }
        }

        for (int i = 0; i < 4; i++) {
            s_rectangle_instance_data[(s_num_draw_rectangles - 1) * s_rectangle_instance_data_num_floats + 16 + i] = s_fill_color[i];
        }
    }
}

void canvash_rotate_2D(float rad) {
    // TODO here we will change current_transform to be rotated
    fprintf(stderr, "[ERROR] cannot call canvash_rotate_2D.\n");
    return;
}

void canvash_fill_color(vec4 color) {
    s_fill = true;
    glm_vec4_copy(color, s_fill_color);
}

void canvash_outline_color(vec4 color) {
    s_outline = true;
    glm_vec4_copy(color, s_outline_color);
}

void canvash_no_fill() {
    s_fill = false;
}

void canvash_no_outline() {
    s_outline = false;
}

void canvash_outline_size(float size) {
    s_outline_size = size;
}

void canvash_background(float *background) {
    glm_vec3_copy(background, s_background);
}

