#include "canvash.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/include/stb_image.h"

// -------------------------------------------------------------

// NOTE glfw and window variables
static GLFWwindow *s_window;
static vec2 s_window_size;
static vec2 s_old_window_size;
static vec2 s_old_window_pos;
static int s_fullscreen = 0;
static void (*s_key_callback)(int, int, int); // key_callback(int key, int scancode, int action)
static void (*s_mouse_callback)(int, int, float, float); // mouse_callback(int button, int action, float xpos, float ypos)

// -------------------------------------------------------------

// NOTE general engine settings
static CanvashMode s_mode;
static int s_init = 0;

// NOTE global transform, passed to each object in its instance data
static vec3 s_current_translate;
static vec3 s_current_rotation;
static vec3 s_current_scale;
static float s_current_rotation_angle;

// NOTE drawing style
static vec4 s_fill_color;
static bool s_fill;
static vec4 s_stroke_color;
static float s_stroke_strength;
static vec3 s_background;

// NOTE rendering variables
//static mat4 s_view; // 3D mode
static mat4 s_proj;
static unsigned int s_num_objects = 0;

//--------------------------------------------------------------

// NOTE rectangle drawing variables
// NOTE rectangles are all instanced, meaning one square is rendered every time and each of them has it's own transform and color
// NOTE scale to match given width and height
// NOTE translate to match corner coordinates
// NOTE color is passed through instance data as well
static int s_num_draw_rectangles = 0;
static float *s_rectangle_instance_data; // mat4x4 transform, vec4 col
static const size_t s_rectangle_instance_data_num_floats = 4 * 4 + 4;
static const size_t s_rectangle_instance_data_size = s_rectangle_instance_data_num_floats * sizeof(float);
static float s_rectangle_object_data_vertices[8] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
};
static unsigned int s_rectangle_object_data_indices[6] = {0, 1, 2, 0, 2, 3};
static Shader s_rectangle_shader;
static GLuint s_rectangle_vbo;
static GLuint s_rectangle_vao;
static GLuint s_rectangle_ebo;
static GLuint s_rectangle_instance_data_vbo;

// -------------------------------------------------------------

// NOTE ellipse drawing variables
// NOTE ellipse is also a circle, so that is included here as well
// NOTE ellipses are all instanced, meaning a square is rendered every time and each of them has it's own transform and color
// NOTE the square is then made into an ellipse in the fragment shader
// NOTE scale to match given radii
// NOTE translate to match center coordinates
// NOTE color is passed through instance data as well
static int s_num_draw_ellipses = 0;
static float *s_ellipse_instance_data; // mat4x4 transform, vec4 col
static const size_t s_ellipse_instance_data_num_floats = 4 * 4 + 4;
static const size_t s_ellipse_instance_data_size = s_ellipse_instance_data_num_floats * sizeof(float);
static float s_ellipse_object_data_vertices[8] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
};
static unsigned int s_ellipse_object_data_indices[6] = {0, 1, 2, 0, 2, 3};
static Shader s_ellipse_shader;
static GLuint s_ellipse_vbo;
static GLuint s_ellipse_vao;
static GLuint s_ellipse_ebo;
static GLuint s_ellipse_instance_data_vbo;

// NOTE ellipse outline rendering variables
// NOTE ellipse outlines have to be rendered using the fragment shader
// NOTE they are significantly more complicated but it's basically the same as ellipses
// NOTE ellipse outline is also a circle outline, so that is included here as well
// NOTE ellipse outlines are all instanced, meaning a square is rendered every time and each of them has it's own transform and color
// NOTE the square is then made into an ellipse outline in the fragment shader
// NOTE ellipse outlines require additional instance data
// NOTE scale to match given radii
// NOTE translate to match center coordinates
// NOTE color is passed through instance data as well
// NOTE width is the thickness of the outlines (also called stroke)
// NOTE size is a vector of the radii of the ellipse, is required for rendering
static int s_num_draw_ellipse_outlines = 0;
static float *s_ellipse_outline_instance_data; // mat4x4 transform, vec4 col, float width, vec2 size
static const size_t s_ellipse_outline_instance_data_size = 4 * 4 * sizeof(float) + 4 * sizeof(float) + 1 * sizeof(float) + 2 * sizeof(float);
static const size_t s_ellipse_outline_instance_data_num_floats = 4 * 4 + 4 + 1 + 2;
static Shader s_ellipse_outline_shader;
static GLuint s_ellipse_outline_vbo;
static GLuint s_ellipse_outline_vao;
static GLuint s_ellipse_outline_ebo;
static GLuint s_ellipse_outline_instance_data_vbo;

// -------------------------------------------------------------

static int s_num_draw_triangles = 0;
static float *s_triangle_object_data;
static const size_t s_triangle_object_data_size = 3 * (3 * sizeof(float) + 4 * 4 * sizeof(float) + 4 * sizeof(float));  /* 3 x (vec3 vertices, mat4x4 transform, vec4 col)*/
static const size_t s_triangle_object_data_num_floats = 3 * (3 + 4 * 4 + 4);  // 3 * (3, 16, 4) floats for 1 triangle
static Shader s_triangle_shader;
static GLuint s_triangle_vbo;
static GLuint s_triangle_vao;

// -------------------------------------------------------------

// credit where credit is due: https://wwwtyro.net/2019/11/18/instanced-lines.html
static int s_num_draw_lines = 0;
static float *s_line_instance_data; /* mat4x4 transform (includes position given as argument), vec4 col, float thickness */
static const size_t s_line_instance_data_size = 4 * 4 * sizeof(float) + 4 * sizeof(float) + sizeof(float);
static const size_t s_line_instance_data_num_floats = 4 * 4 + 4 + 1;
static float s_line_object_data_vertices[12] = {
        0.0f, -0.5f,
        1.0f, -0.5f,
        1.0f, 0.5f,
        0.0f, -0.5f,
        0.0f, 0.5f,
        1.0f, 0.5f
};
static Shader s_line_shader;
static GLuint s_line_vbo;
static GLuint s_line_vao;
static GLuint s_line_instance_data_vbo;

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
        if (s_fullscreen) {
            glfwSetWindowMonitor(window, NULL, s_old_window_pos[0], s_old_window_pos[1], (int) s_old_window_size[0], (int) s_old_window_size[1], GLFW_DONT_CARE);
            s_fullscreen = 0;
        } else {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            s_old_window_size[0] = s_window_size[0];
            s_old_window_size[1] = s_window_size[1];
            int pos_x, pos_y;
            glfwGetWindowPos(window, &pos_x, &pos_y);
            s_old_window_pos[0] = (float) pos_x;
            s_old_window_pos[1] = (float) pos_y;
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            s_fullscreen = 1;
        }
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
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
    glEnable(GL_DEPTH_TEST);
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

    s_rectangle_shader = create_shader("res/shader/rect_vert.glsl", "res/shader/rect_frag.glsl");
    glGenVertexArrays(1, &s_rectangle_vao);
    glGenBuffers(1, &s_rectangle_vbo);
    glGenBuffers(1, &s_rectangle_ebo);
    glGenBuffers(1, &s_rectangle_instance_data_vbo);

    s_ellipse_shader = create_shader("res/shader/ell_vert.glsl", "res/shader/ell_frag.glsl");
    glGenVertexArrays(1, &s_ellipse_vao);
    glGenBuffers(1, &s_ellipse_vbo);
    glGenBuffers(1, &s_ellipse_ebo);
    glGenBuffers(1, &s_ellipse_instance_data_vbo);

    s_ellipse_outline_shader = create_shader("res/shader/ell_ol_vert.glsl", "res/shader/ell_ol_frag.glsl");
    glGenVertexArrays(1, &s_ellipse_outline_vao);
    glGenBuffers(1, &s_ellipse_outline_vbo);
    glGenBuffers(1, &s_ellipse_outline_ebo);
    glGenBuffers(1, &s_ellipse_outline_instance_data_vbo);

    s_triangle_shader = create_shader("res/shader/tri_vert.glsl", "res/shader/tri_frag.glsl");
    glGenVertexArrays(1, &s_triangle_vao);
    glGenBuffers(1, &s_triangle_vbo);

    s_line_shader = create_shader("res/shader/line_vert.glsl", "res/shader/line_frag.glsl");
    glGenVertexArrays(1, &s_line_vao);
    glGenBuffers(1, &s_line_vbo);
    glGenBuffers(1, &s_line_instance_data_vbo);

    // NOTE set all of the styling data to basic (very gray and boring)
    glm_vec3_zero(s_current_translate);
    glm_vec3_copy((vec3) {0.0f, 0.0f, 1.0f}, s_current_rotation);
    glm_vec3_one(s_current_scale);
    s_current_rotation_angle = 0.0f;

    glm_mat4_identity(s_proj);
    glm_vec4_one(s_fill_color); // white
    glm_vec4_zero(s_stroke_color); // black
    s_stroke_color[3] = 1.0f;
    s_fill = true; // fill
    s_stroke_strength = 5.0f; // none
    s_background[0] = s_background[1] = s_background[2] = 0.5f; // gray

    s_init = 1;
    return 1;
}

int canvash_running() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_running() without initialization first. call canvash_init() before doing anything else.\n");
        return 0;
    }
    return !glfwWindowShouldClose(s_window);
}

void canvash_clear_screen() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_clear_screen() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(s_background[0], s_background[1], s_background[2], 1.0f);
}

void canvash_render() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_render() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }
    glm_ortho(-s_window_size[0] / 2.0f, s_window_size[0] / 2.0f, -s_window_size[1] / 2.0f, s_window_size[1] / 2.0f, -1.0f, 1.0f, s_proj);
//    glm_ortho(-s_window_size[0] / s_window_size[1], s_window_size[0] / s_window_size[1], -1.0f, 1.0f, -1.0f, 1.0f, s_proj);

    if (s_rectangle_instance_data && s_num_draw_rectangles) {

        float rectangle_object_data_vertices_new[12];

        int ind = 0;
        for (int i = 0; i < 4; i++) {
            rectangle_object_data_vertices_new[i * 3 + 0] = s_rectangle_object_data_vertices[ind++];
            rectangle_object_data_vertices_new[i * 3 + 1] = s_rectangle_object_data_vertices[ind++];
            rectangle_object_data_vertices_new[i * 3 + 2] = 0;
        }

        {
            glBindVertexArray(s_rectangle_vao);

            glBindBuffer(GL_ARRAY_BUFFER, s_rectangle_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_object_data_vertices_new), rectangle_object_data_vertices_new, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_rectangle_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_rectangle_object_data_indices), s_rectangle_object_data_indices, GL_STATIC_DRAW);

            // NOTE this gives the position of the base quad vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
            glEnableVertexAttribArray(0);

            // NOTE this is different for each quad/rect
            glBindBuffer(GL_ARRAY_BUFFER, s_rectangle_instance_data_vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) s_rectangle_instance_data_size * s_num_draw_rectangles, s_rectangle_instance_data, GL_STATIC_DRAW);

            // NOTE this is the model matrix
            for (unsigned int i = 0; i < 4; i++) {
                glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_rectangle_instance_data_num_floats * sizeof(GLfloat)), (void *) (sizeof(GLfloat) * i * 4));
                glEnableVertexAttribArray(1 + i);
                glVertexAttribDivisor(1 + i, 1);
            }

            // NOTE this is the color
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_rectangle_instance_data_num_floats * sizeof(GLfloat)), (void *) (16 * sizeof(GLfloat)));
            glEnableVertexAttribArray(5);
            glVertexAttribDivisor(5, 1); // This also advances once per instance

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            activate_shader(s_rectangle_shader);
            glBindVertexArray(s_rectangle_vao);

            shader_upload_m4(s_rectangle_shader, "u_proj", s_proj);

            // glDrawArraysInstanced(GL_TRIANGLES, 0, 4, numInstances);
            glDrawElementsInstanced(GL_TRIANGLES, sizeof(s_rectangle_object_data_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0, s_num_draw_rectangles);

            deactivate_shader();
            glBindVertexArray(0);
        }

        free((void *) s_rectangle_instance_data);
        s_rectangle_instance_data = NULL;
        s_num_draw_rectangles = 0;
    }

    if (s_ellipse_instance_data && s_num_draw_ellipses) {

        float ellipse_object_data_vertices_new[12];

        int ind = 0;
        for (int i = 0; i < 4; i++) {
            ellipse_object_data_vertices_new[i * 3 + 0] = s_ellipse_object_data_vertices[ind++];
            ellipse_object_data_vertices_new[i * 3 + 1] = s_ellipse_object_data_vertices[ind++];
            ellipse_object_data_vertices_new[i * 3 + 2] = 0;
        }

        {
            glBindVertexArray(s_ellipse_vao);

            glBindBuffer(GL_ARRAY_BUFFER, s_ellipse_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ellipse_object_data_vertices_new), ellipse_object_data_vertices_new, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ellipse_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_ellipse_object_data_indices), s_ellipse_object_data_indices, GL_STATIC_DRAW);

            // NOTE this gives the position of the base quad vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
            glEnableVertexAttribArray(0);

            // NOTE this is different for each ellipse
            glBindBuffer(GL_ARRAY_BUFFER, s_ellipse_instance_data_vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) s_ellipse_instance_data_size * s_num_draw_ellipses, s_ellipse_instance_data, GL_STATIC_DRAW);

            // NOTE this is the model matrix
            for (unsigned int i = 0; i < 4; i++) {
                glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_ellipse_instance_data_num_floats * sizeof(GLfloat)), (void *) (sizeof(GLfloat) * i * 4));
                glEnableVertexAttribArray(1 + i);
                glVertexAttribDivisor(1 + i, 1);
            }

            // NOTE this is the color
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_ellipse_instance_data_num_floats * sizeof(GLfloat)), (void *) (16 * sizeof(GLfloat)));
            glEnableVertexAttribArray(5);
            glVertexAttribDivisor(5, 1); // This also advances once per instance

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            activate_shader(s_ellipse_shader);
            glBindVertexArray(s_ellipse_vao);

            shader_upload_m4(s_ellipse_shader, "u_proj", s_proj);
            shader_upload_v2(s_ellipse_shader, "u_screen_size", s_window_size);

            // glDrawArraysInstanced(GL_TRIANGLES, 0, 4, numInstances);
            glDrawElementsInstanced(GL_TRIANGLES, sizeof(s_ellipse_object_data_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0, s_num_draw_ellipses);

            deactivate_shader();
            glBindVertexArray(0);
        }

        free((void *) s_ellipse_instance_data);
        s_ellipse_instance_data = NULL;
        s_num_draw_ellipses = 0;
    }

    if (s_ellipse_outline_instance_data && s_num_draw_ellipse_outlines) {

        float ellipse_outline_object_data_vertices_new[12];

        int ind = 0;
        for (int i = 0; i < 4; i++) {
            ellipse_outline_object_data_vertices_new[i * 3 + 0] = s_ellipse_object_data_vertices[ind++];
            ellipse_outline_object_data_vertices_new[i * 3 + 1] = s_ellipse_object_data_vertices[ind++];
            ellipse_outline_object_data_vertices_new[i * 3 + 2] = 0;
        }

        {
            glBindVertexArray(s_ellipse_outline_vao);

            glBindBuffer(GL_ARRAY_BUFFER, s_ellipse_outline_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ellipse_outline_object_data_vertices_new), ellipse_outline_object_data_vertices_new, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ellipse_outline_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_ellipse_object_data_indices), s_ellipse_object_data_indices, GL_STATIC_DRAW);

            // NOTE this gives the position of the base quad vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
            glEnableVertexAttribArray(0);

            // NOTE this is different for each ellipse_outline
            glBindBuffer(GL_ARRAY_BUFFER, s_ellipse_outline_instance_data_vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) s_ellipse_outline_instance_data_size * s_num_draw_ellipse_outlines, s_ellipse_outline_instance_data, GL_STATIC_DRAW);

            // NOTE this is the model matrix
            for (unsigned int i = 0; i < 4; i++) {
                glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_ellipse_outline_instance_data_num_floats * sizeof(GLfloat)), (void *) (sizeof(GLfloat) * i * 4));
                glEnableVertexAttribArray(1 + i);
                glVertexAttribDivisor(1 + i, 1);
            }

            // NOTE this is the color
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_ellipse_outline_instance_data_num_floats * sizeof(GLfloat)), (void *) (16 * sizeof(GLfloat)));
            glEnableVertexAttribArray(5);
            glVertexAttribDivisor(5, 1); // This also advances once per instance

            // NOTE this is the stroke width
            glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, (GLsizei) (s_ellipse_outline_instance_data_num_floats * sizeof(GLfloat)), (void *) (20 * sizeof(GLfloat)));
            glEnableVertexAttribArray(6);
            glVertexAttribDivisor(6, 1);

            glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, (GLsizei) (s_ellipse_outline_instance_data_num_floats * sizeof(GLfloat)), (void *) (21 * sizeof(GLfloat)));
            glEnableVertexAttribArray(7);
            glVertexAttribDivisor(7, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            activate_shader(s_ellipse_outline_shader);
            glBindVertexArray(s_ellipse_outline_vao);

            shader_upload_m4(s_ellipse_outline_shader, "u_proj", s_proj);
            shader_upload_v2(s_ellipse_outline_shader, "u_screen_size", s_window_size);

            // glDrawArraysInstanced(GL_TRIANGLES, 0, 4, numInstances);
            glDrawElementsInstanced(GL_TRIANGLES, sizeof(s_ellipse_object_data_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0, s_num_draw_ellipse_outlines);

            deactivate_shader();
            glBindVertexArray(0);
        }

        free((void *) s_ellipse_outline_instance_data);
        s_ellipse_outline_instance_data = NULL;
        s_num_draw_ellipse_outlines = 0;
    }

    if (s_triangle_object_data && s_num_draw_triangles) {

        {
            glBindVertexArray(s_triangle_vao);

            glBindBuffer(GL_ARRAY_BUFFER, s_triangle_vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) s_triangle_object_data_size * s_num_draw_triangles, s_triangle_object_data, GL_STATIC_DRAW);

            // NOTE this gives the position of the base triangle vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat) + 4 * 4 * sizeof(GLfloat) + 4 * sizeof(GLfloat), (void *) 0);
            glEnableVertexAttribArray(0);

            // NOTE this is the model matrix
            for (unsigned int i = 0; i < 4; i++) {
                glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat) + 4 * 4 * sizeof(GLfloat) + 4 * sizeof(GLfloat), (void *) (sizeof(GLfloat) * 3 + sizeof(GLfloat) * i * 4));
                glEnableVertexAttribArray(1 + i);
            }

            // NOTE this is the color
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat) + 4 * 4 * sizeof(GLfloat) + 4 * sizeof(GLfloat), (void *) (sizeof(GLfloat) * 3 + 16 * sizeof(GLfloat)));
            glEnableVertexAttribArray(5);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            activate_shader(s_triangle_shader);
            glBindVertexArray(s_triangle_vao);

            shader_upload_m4(s_triangle_shader, "u_proj", s_proj);

            glDrawArrays(GL_TRIANGLES, 0, s_num_draw_triangles * 3);

            deactivate_shader();
            glBindVertexArray(0);
        }

        free((void *) s_triangle_object_data);
        s_triangle_object_data = NULL;
        s_num_draw_triangles = 0;
    }

    if (s_line_instance_data && s_num_draw_lines) {

        float line_object_data_vertices_new[18];

        int ind = 0;
        for (int i = 0; i < 6; i++) {
            line_object_data_vertices_new[i * 3 + 0] = s_line_object_data_vertices[ind++];
            line_object_data_vertices_new[i * 3 + 1] = s_line_object_data_vertices[ind++];
            line_object_data_vertices_new[i * 3 + 2] = 0;
        }

        {
            glBindVertexArray(s_line_vao);

            glBindBuffer(GL_ARRAY_BUFFER, s_line_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(line_object_data_vertices_new), line_object_data_vertices_new, GL_STATIC_DRAW);

            // NOTE this gives the position of the base quad vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
            glEnableVertexAttribArray(0);

            // NOTE this is different for each
            glBindBuffer(GL_ARRAY_BUFFER, s_line_instance_data_vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) s_line_instance_data_size * s_num_draw_lines, s_line_instance_data, GL_STATIC_DRAW);

            // NOTE this is the model matrix
            for (unsigned int i = 0; i < 4; i++) {
                glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_line_instance_data_num_floats * sizeof(GLfloat)), (void *) (sizeof(GLfloat) * i * 4));
                glEnableVertexAttribArray(1 + i);
                glVertexAttribDivisor(1 + i, 1);
            }

            // NOTE this is the color
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, (GLsizei) (s_line_instance_data_num_floats * sizeof(GLfloat)), (void *) (16 * sizeof(GLfloat)));
            glEnableVertexAttribArray(5);
            glVertexAttribDivisor(5, 1);

            // NOTE this is the line thickness / stroke strength
            glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, (GLsizei) (s_line_instance_data_num_floats * sizeof(GLfloat)), (void *) ((16 + 4) * sizeof(GLfloat)));
            glEnableVertexAttribArray(6);
            glVertexAttribDivisor(6, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            activate_shader(s_line_shader);
            glBindVertexArray(s_line_vao);

            shader_upload_m4(s_line_shader, "u_proj", s_proj);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, s_num_draw_lines);

            deactivate_shader();
            glBindVertexArray(0);
        }

        free((void *) s_line_instance_data);
        s_line_instance_data = NULL;
        s_num_draw_lines = 0;
    }

    // NOTE reset everything
    glm_vec3_zero(s_current_translate);
    glm_vec3_copy((vec3) {0.0f, 0.0f, 1.0f}, s_current_rotation);
    glm_vec3_one(s_current_scale);
    s_current_rotation_angle = 0.0f;

    glm_mat4_identity(s_proj);
    glm_vec4_one(s_fill_color); // white
    glm_vec4_zero(s_stroke_color); // black
    s_stroke_color[3] = 1.0f;
    s_fill = true; // fill
    s_stroke_strength = 5.0f; // none
    s_num_objects = 0;

    glfwSwapBuffers(s_window);
    glfwPollEvents();
}

void canvash_terminate() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_terminate() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    delete_shader(s_rectangle_shader);
    glDeleteVertexArrays(1, &s_rectangle_vao);
    glDeleteBuffers(1, &s_rectangle_vbo);
    glDeleteBuffers(1, &s_rectangle_ebo);
    glDeleteBuffers(1, &s_rectangle_instance_data_vbo);

    delete_shader(s_ellipse_shader);
    glDeleteVertexArrays(1, &s_ellipse_vao);
    glDeleteBuffers(1, &s_ellipse_vbo);
    glDeleteBuffers(1, &s_ellipse_ebo);
    glDeleteBuffers(1, &s_ellipse_instance_data_vbo);

    delete_shader(s_ellipse_outline_shader);
    glDeleteVertexArrays(1, &s_ellipse_outline_vao);
    glDeleteBuffers(1, &s_ellipse_outline_vbo);
    glDeleteBuffers(1, &s_ellipse_outline_ebo);
    glDeleteBuffers(1, &s_ellipse_outline_instance_data_vbo);

    delete_shader(s_triangle_shader);
    glDeleteVertexArrays(1, &s_triangle_vao);
    glDeleteBuffers(1, &s_triangle_vbo);

    delete_shader(s_line_shader);
    glDeleteVertexArrays(1, &s_line_vao);
    glDeleteBuffers(1, &s_line_vbo);
    glDeleteBuffers(1, &s_line_instance_data_vbo);

//    free((void *) s_rectangle_instance_data);
//    free((void *) s_ellipse_instance_data);

    glfwDestroyWindow(s_window);
    glfwTerminate();

    s_init = 0;
}

// -------------------------------------------------------------

void canvash_set_key_callback(void (*key_callback)(int, int, int)) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_set_key_callback() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_key_callback = key_callback;
}

void canvash_set_mouse_callback(void (*mouse_callback)(int, int, float, float)) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_set_mouse_callback() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_mouse_callback = mouse_callback;
}

void canvash_get_window_size(float *width, float *height) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_get_window_size() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    *width = s_window_size[0];
    *height = s_window_size[1];
}

// -------------------------------------------------------------

void canvash_rectangle_2D(vec2 p1, vec2 p2) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_rectangle_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    if (s_num_objects * 2 + 2 >= CANVASH_MAX_OBJECTS) {
        fprintf(stderr, "[ERROR] exceeded limit of objects to draw (max: %d). will not draw anymore.\n", CANVASH_MAX_OBJECTS);
        return;
    }

    if (s_mode == threedimensional) {
        fprintf(stderr, "[ERROR] cannot call canvash_rectangle_2D while in threedimensional mode.\n");
        return;
    }

    if (s_fill) {
        // NOTE renders the fill mesh
        s_num_objects++;

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
        glm_mat4_identity(transform);

        vec2 center;
        glm_vec2_add(p1, p2, center);
        glm_vec2_scale(center, 0.5f, center);

        // NOTE z-indexing by changing the z translation (should work fine)

        // NOTE this comes before translation since we WANT this to be rotated/scaled weirdly because we want it to rotate around (0,0), not it's center
        glm_rotate(transform, s_current_rotation_angle, s_current_rotation);
        glm_scale(transform, s_current_scale);

        glm_translate(transform, (vec3) {center[0], center[1], -1.0f + (float) s_num_objects / (float) CANVASH_MAX_OBJECTS});
        glm_translate(transform, s_current_translate);

        // NOTE will scale by the width and height and since the initial size is unitary this will translate into pixels on the screen
        glm_scale(transform, (vec3) {fabsf(p2[0] - p1[0]), fabsf(p2[1] - p1[1]), 1.0f});


        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                s_rectangle_instance_data[(s_num_draw_rectangles - 1) * s_rectangle_instance_data_num_floats + i * 4 + j] = transform[i][j];
            }
        }

        for (int i = 0; i < 4; i++) {
            s_rectangle_instance_data[(s_num_draw_rectangles - 1) * s_rectangle_instance_data_num_floats + 16 + i] = s_fill_color[i];
        }
    }

    // NOTE renders the outline
    if (s_stroke_strength != 0.0f) {
        canvash_line_2D(p1, (vec2){p2[0], p1[1]});
        canvash_line_2D((vec2){p2[0], p1[1]}, p2);
        canvash_line_2D(p2, (vec2){p1[0], p2[1]});
        canvash_line_2D((vec2){p1[0], p2[1]}, p1);
    }
}

void canvash_ellipse_2D(float *p, float a, float b) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_ellipse_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    if (s_num_objects * 2 + 2 >= CANVASH_MAX_OBJECTS) {
        fprintf(stderr, "[ERROR] exceeded limit of objects to draw (max: %d). will not draw anymore.\n", CANVASH_MAX_OBJECTS);
        return;
    }

    if (s_mode == threedimensional) {
        fprintf(stderr, "[ERROR] cannot call canvash_ellipse_2D while in threedimensional mode.\n");
        return;
    }

    if (a <= 0.0f || b <= 0.0f) {
        fprintf(stderr, "[ERROR] stop trying to be funny, an ellipse cannot have negative radii (a = %f, b = %f).\n", a, b);
        return;
    }

    // NOTE this wouldn't make sense anyway and it causes problems with the ellipse outline
    if (s_stroke_strength / 2.0f > a || s_stroke_strength / 2.0f > b) {
        fprintf(stderr, "[ERROR] cannot call canvash_ellipse_2D while half of stroke_strength is larger or equal to the radii of the ellipse (a = %f, b = %f, stroke = %f).\n", a, b, s_stroke_strength);
        return;
    }

    if (s_fill) {
        // NOTE renders the fill mesh
        s_num_objects++;

        // NOTE resize the current instance data to fit with the added ellipse
        s_num_draw_ellipses++;
        if (!s_ellipse_instance_data && !s_num_draw_ellipses) {
            s_ellipse_instance_data = (float *) malloc(s_ellipse_instance_data_size * s_num_draw_ellipses);
            if (s_ellipse_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to allocate any ellipse instance data.\n");
                s_num_draw_ellipses--;
                return;
            }
        } else {
            float *new_ellipse_instance_data = (float *) realloc(s_ellipse_instance_data, s_ellipse_instance_data_size * s_num_draw_ellipses);
            if (new_ellipse_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to reallocate new ellipse instance data.\n");
                s_num_draw_ellipses--;
                return;
            } else {
                s_ellipse_instance_data = new_ellipse_instance_data;
            }
        }

        // NOTE add the instance data
        // NOTE 16 floats for transform matrix + 4 floats for color

        mat4 transform;
        glm_mat4_identity(transform);

        vec2 center;
        glm_vec2_copy(p, center);

        // NOTE this comes before translation since we WANT this to be rotated/scaled weirdly because we want it to rotate around (0,0), not it's center
        glm_rotate(transform, s_current_rotation_angle, s_current_rotation);
        glm_scale(transform, s_current_scale);

        glm_translate(transform, (vec3) {center[0], center[1], -1.0f + (float) s_num_objects / (float) CANVASH_MAX_OBJECTS});
        glm_translate(transform, s_current_translate);
        // NOTE will scale by the r1 and r2 (times 2 because of diameter / radius clarity - all of this is in radii) and since the initial size is unitary this will translate into pixels on the screen
        glm_scale(transform, (vec3) {2.0f*a, 2.0f*b, 1.0f});

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                s_ellipse_instance_data[(s_num_draw_ellipses - 1) * s_ellipse_instance_data_num_floats + i * 4 + j] = transform[i][j];
            }
        }

        for (int i = 0; i < 4; i++) {
            s_ellipse_instance_data[(s_num_draw_ellipses - 1) * s_ellipse_instance_data_num_floats + 16 + i] = s_fill_color[i];
        }
    }

    if (s_stroke_strength != 0.0f) {
        s_num_objects++;

        // NOTE resize the current instance data to fit with the added ellipse_outline
        s_num_draw_ellipse_outlines++;
        if (!s_ellipse_outline_instance_data && !s_num_draw_ellipse_outlines) {
            s_ellipse_outline_instance_data = (float *) malloc(s_ellipse_outline_instance_data_size * s_num_draw_ellipse_outlines);
            if (s_ellipse_outline_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to allocate any ellipse_outline instance data.\n");
                s_num_draw_ellipse_outlines--;
                return;
            }
        } else {
            float *new_ellipse_outline_instance_data = (float *) realloc(s_ellipse_outline_instance_data, s_ellipse_outline_instance_data_size * s_num_draw_ellipse_outlines);
            if (new_ellipse_outline_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to reallocate new ellipse_outline instance data.\n");
                s_num_draw_ellipse_outlines--;
                return;
            } else {
                s_ellipse_outline_instance_data = new_ellipse_outline_instance_data;
            }
        }

        // NOTE add the instance data
        // NOTE 16 floats for transform matrix + 4 floats for color

        mat4 transform;
        glm_mat4_identity(transform);

        vec2 center;
        glm_vec2_copy(p, center);

        // NOTE this comes before translation since we WANT this to be rotated/scaled weirdly because we want it to rotate around (0,0), not it's center
        glm_rotate(transform, s_current_rotation_angle, s_current_rotation);
        glm_scale(transform, s_current_scale);

        glm_translate(transform, (vec3) {center[0], center[1], -1.0f + (float) s_num_objects / (float) CANVASH_MAX_OBJECTS});
        glm_translate(transform, s_current_translate);

        // NOTE an interesting condition that works for the ellipse outline sdf in the fragment shader
        if (a > b)
        glm_scale(transform, (vec3) {2.0f*a, 2.0f*a, 1.0f});
        else
        glm_scale(transform, (vec3) {2.0f*b, 2.0f*b, 1.0f});

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                s_ellipse_outline_instance_data[(s_num_draw_ellipse_outlines - 1) * s_ellipse_outline_instance_data_num_floats + i * 4 + j] = transform[i][j];
            }
        }

        for (int i = 0; i < 4; i++) {
            s_ellipse_outline_instance_data[(s_num_draw_ellipse_outlines - 1) * s_ellipse_outline_instance_data_num_floats + 16 + i] = s_stroke_color[i];
        }

        // NOTE stroke strength goes to the gpu as well as the ellipse dimensions
        s_ellipse_outline_instance_data[(s_num_draw_ellipse_outlines - 1) * s_ellipse_outline_instance_data_num_floats + 16 + 4 + 0] = s_stroke_strength;
        s_ellipse_outline_instance_data[(s_num_draw_ellipse_outlines - 1) * s_ellipse_outline_instance_data_num_floats + 16 + 5 + 0] = 2.0f*a;
        s_ellipse_outline_instance_data[(s_num_draw_ellipse_outlines - 1) * s_ellipse_outline_instance_data_num_floats + 16 + 5 + 1] = 2.0f*b;
    }
}

void canvash_circle_2D(float *p, float r) {
    // NOTE just like the ellipse but put safeguards before so that the correct function name is used in the error log

    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_circle_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    if (s_num_objects * 2 + 2 >= CANVASH_MAX_OBJECTS) {
        fprintf(stderr, "[ERROR] exceeded limit of objects to draw (max: %d). will not draw anymore.\n", CANVASH_MAX_OBJECTS);
        return;
    }

    if (s_mode == threedimensional) {
        fprintf(stderr, "[ERROR] cannot call canvash_circle_2D while in threedimensional mode.\n");
        return;
    }

    if (r <= 0.0f) {
        fprintf(stderr, "[ERROR] stop trying to be funny, an ellipse cannot have a negative radius (r = %f).\n", r);
        return;
    }

    // NOTE this wouldn't make sense anyway and it causes problems with the ellipse outline
    if (s_stroke_strength / 2.0f > r) {
        fprintf(stderr, "[ERROR] cannot call canvash_circle_2D while half of stroke_strength is larger or equal to the radius (r = %f, stroke = %f).\n", r, s_stroke_strength);
        return;
    }

    canvash_ellipse_2D(p, r, r);
}

void canvash_triangle_2D(float *p1, float *p2, float *p3) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_triangle_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    if (s_num_objects * 2 + 2 >= CANVASH_MAX_OBJECTS) {
        fprintf(stderr, "[ERROR] exceeded limit of objects to draw (max: %d). will not draw anymore.\n", CANVASH_MAX_OBJECTS);
        return;
    }

    if (s_mode == threedimensional) {
        fprintf(stderr, "[ERROR] cannot call canvash_triangle_2D while in threedimensional mode.\n");
        return;
    }

    // NOTE renders the fill mesh
    if (s_fill) {
        s_num_objects++;

        // NOTE resize the current instance data to fit with the added triangle
        s_num_draw_triangles++;

        if (!s_triangle_object_data && !s_num_draw_triangles) {
            s_triangle_object_data = (float *) malloc(s_triangle_object_data_size * s_num_draw_triangles);
            if (s_triangle_object_data == NULL) {
                fprintf(stderr, "[ERROR] failed to allocate any triangle object data.\n");
                s_num_draw_triangles--;
                return;
            }
        } else {
            float *new_triangle_object_data = (float *) realloc(s_triangle_object_data, s_triangle_object_data_size * s_num_draw_triangles);
            if (new_triangle_object_data == NULL) {
                fprintf(stderr, "[ERROR] failed to reallocate new triangle object data.\n");
                s_num_draw_triangles--;
                return;
            } else {
                s_triangle_object_data = new_triangle_object_data;
            }
        }

        // NOTE by no means effective or elegant, but tf do i care
        {
            int ind = 0;
            mat4 transform;
            glm_mat4_identity(transform);

            glm_translate(transform, s_current_translate);
            glm_translate(transform, (vec3) {0.0f, 0.0f, -1.0f + (float) s_num_objects / (float) CANVASH_MAX_OBJECTS});
            glm_rotate(transform, s_current_rotation_angle, s_current_rotation);
            glm_scale(transform, s_current_scale);

            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = p1[0];
            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = p1[1];
            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = 0.0f;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = transform[i][j];
                }
            }

            for (int i = 0; i < 4; i++) {
                s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = s_fill_color[i];
            }

            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = p2[0];
            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = p2[1];
            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = 0.0f;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = transform[i][j];
                }
            }

            for (int i = 0; i < 4; i++) {
                s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = s_fill_color[i];
            }

            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = p3[0];
            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = p3[1];
            s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = 0.0f;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = transform[i][j];
                }
            }

            for (int i = 0; i < 4; i++) {
                s_triangle_object_data[(s_num_draw_triangles - 1) * s_triangle_object_data_num_floats + ind++] = s_fill_color[i];
            }
        }
    }

    // NOTE renders the outlines
    if (s_stroke_strength != 0.0f) {
        canvash_line_2D(p1, p2);
        canvash_line_2D(p2, p3);
        canvash_line_2D(p3, p1);
    }
}

void canvash_line_2D(float *p1, float *p2) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_line_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    if (s_num_objects * 2 + 2 >= CANVASH_MAX_OBJECTS) {
        fprintf(stderr, "[ERROR] exceeded limit of objects to draw (max: %d). will not draw anymore.\n", CANVASH_MAX_OBJECTS);
        return;
    }

    if (s_mode == threedimensional) {
        fprintf(stderr, "[ERROR] cannot call canvash_line_2D while in threedimensional mode.\n");
        return;
    }

    if (s_stroke_strength == 0.0f) {
        fprintf(stderr, "[ERROR] cannot call canvash_line_2D while stroke_strength is zero.\n");
        return;
    }

    // NOTE these are the caps, it's primitive... but... the grass also doesn't look very pink to me... so yes...
    {
        float stroke_strength = s_stroke_strength;
        s_stroke_strength = 0.0f;
        vec3 fill;
        glm_vec3_copy(s_fill_color, fill);
        glm_vec3_copy(s_stroke_color, s_fill_color);

        canvash_circle_2D(p1, stroke_strength / 2.0f);
        canvash_circle_2D(p2, stroke_strength / 2.0f);

        glm_vec3_copy(fill, s_fill_color);
        s_stroke_strength = stroke_strength;
    }

    {
        // NOTE renders the fill mesh
        s_num_objects++;

        // NOTE resize the current instance data to fit with the added line
        s_num_draw_lines++;
        if (!s_line_instance_data && !s_num_draw_lines) {
            s_line_instance_data = (float *) malloc(s_line_instance_data_size * s_num_draw_lines);
            if (s_line_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to allocate any line instance data.\n");
                s_num_draw_lines--;
                return;
            }
        } else {
            float *new_line_instance_data = (float *) realloc(s_line_instance_data, s_line_instance_data_size * s_num_draw_lines);
            if (new_line_instance_data == NULL) {
                fprintf(stderr, "[ERROR] failed to reallocate new line instance data.\n");
                s_num_draw_lines--;
                return;
            } else {
                s_line_instance_data = new_line_instance_data;
            }
        }

        // NOTE add the instance data
        // NOTE 16 floats for transform matrix + 4 floats for color + 1 for strength
        mat4 transform;
        glm_mat4_identity(transform);

        glm_rotate(transform, s_current_rotation_angle, s_current_rotation);
        glm_scale(transform, s_current_scale);

        glm_translate(transform, s_current_translate);
        glm_translate(transform, (vec3) {0.0f, 0.0f, -1.0f + (float) s_num_objects / (float) CANVASH_MAX_OBJECTS});

        // NOTE line-specific translation
        vec2 dir;
        glm_vec2_sub(p2, p1, dir);
        glm_vec2_normalize(dir);

        glm_translate(transform, (vec3){p1[0], p1[1], 0.0f});

        // NOTE line-specific rotation
        float theta = atan2f(p2[1]-p1[1], p2[0]-p1[0]);
        glm_rotate(transform, theta, s_current_rotation);

        // NOTE will scale by the width and height and since the initial size is unitary this will translate into pixels on the screen
        // NOTE line-specific scale
        vec3 scale;
        glm_vec3_fill(scale, glm_vec2_distance(p1, p2));
        scale[1] = 1.0f;
        scale[2] = 1.0f;
        glm_scale(transform, scale);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                s_line_instance_data[(s_num_draw_lines - 1) * s_line_instance_data_num_floats + i * 4 + j] = transform[i][j];
            }
        }

        for (int i = 0; i < 4; i++) {
            s_line_instance_data[(s_num_draw_lines - 1) * s_line_instance_data_num_floats + 16 + i] = s_stroke_color[i];
        }

        s_line_instance_data[(s_num_draw_lines - 1) * s_line_instance_data_num_floats + 16 + 3 + 1] = s_stroke_strength;
    }
}

// -------------------------------------------------------------

void canvash_rotate_2D(float rad) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_rotate_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_current_rotation_angle = rad;
}

void canvash_scale_2D(float factor_x, float factor_y) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_scale_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_current_scale[0] *= factor_x;
    s_current_scale[1] *= factor_y;
}

void canvash_translate_2D(float move_x, float move_y) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_translate_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_current_translate[0] += move_x;
    s_current_translate[1] += move_y;
}

void canvash_reset_transform_2D() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_reset_transform_2D() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    glm_vec3_zero(s_current_translate);
    glm_vec3_copy((vec3) {0.0f, 0.0f, 1.0f}, s_current_rotation);
    glm_vec3_one(s_current_scale);
    s_current_rotation_angle = 0.0f;
}


// -------------------------------------------------------------

void canvash_fill_color(vec4 color) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_fill_color() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_fill = true;
    glm_vec4_copy(color, s_fill_color);
}

void canvash_no_fill() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_no_fill() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_fill = false;
}

void canvash_stroke_color(vec4 color) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_stroke_color() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    glm_vec4_copy(color, s_stroke_color);
}

void canvash_no_stroke() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_no_stroke() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_stroke_strength = 0.0f;
}

void canvash_stroke(float strength) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_stroke() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    s_stroke_strength = strength;
}

// -------------------------------------------------------------

void canvash_background(float *background) {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_background() without initialization first. call canvash_init() before doing anything else.\n");
        return;
    }

    glm_vec3_copy(background, s_background);
}

float canvash_time() {
    if (!s_init) {
        fprintf(stderr, "[ERROR] can't call canvash_time() without initialization first. call canvash_init() before doing anything else.\n");
        return 0.0f;
    }

    return (float) glfwGetTime() * 10.0f;
}
