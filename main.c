#include "canvash_lib/canvash.h"

// NOTE this is a simple key callback function, modify it to your liking
void key_callback(int key, int scancode, int action) {
    if (CANVASH_KEY_IS_PRESSED(key, action, CANVASH_KEY_E)) {
        printf("Hewwo, key callback work!\n");
    }
}
// NOTE this is a simple mouse callback function, modify it to your liking
void mouse_callback(int button, int action, float xpos, float ypos) {
    if (CANVASH_BUTTON_IS_PRESSED(button, action, CANVASH_MOUSE_BUTTON_LEFT)) {
        printf("Hewwo, mouse callback work at %f %f!\n", xpos, ypos);
    }
}

int main() {
    // NOTE initialize canvash with size 800x600, title "canvash_dev", with an icon, 2D mode (check if it failed) and set the key and mouse callback
    if (!canvash_init("canvash_dev", 800, 600, "res/img/icon.png", twodimensional)) {
        printf("[USER] failed initialization.\n");
        return -1;
    }
    canvash_set_key_callback(key_callback);
    canvash_set_mouse_callback(mouse_callback);
    canvash_background((vec3){0.2f, 0.3f, 0.3f});

    // NOTE this is your main loop, it's like the draw() function in p5.js
    while (canvash_running()) {
        // NOTE essential (do not leave out)
        canvash_clear_screen();

        canvash_background((vec3){0.2f, 0.3f, (float)canvash_time()/200.0f});


        canvash_stroke_color((vec4){1.0f, 0.0f, 1.0f, 1.0f});
        canvash_fill_color((vec4){0.0f, 1.0f, 0.0f, 1.0f});

//        canvash_rotate_2D(glm_rad(glfwGetTime()) * 20.10f);

//        canvash_triangle_2D((vec2){-50.0f, 150.0f}, (vec2){0.0f, 200.0f}, (vec2){50.0f, 150.0f});

        canvash_fill_color((vec4){0.9f, 0.5f, 0.1f, 1.0f});

//        canvash_rectangle_2D((vec2){-25.0f, 25.0f}, (vec2){25.0f, 75.0f});


        canvash_stroke(30.0f);

//        canvash_circle_2D((vec2){100.0f, 0.0f}, 100.0f);
//        canvash_circle_2D((vec2){-100.0f, 0.0f}, 100.0f);
        // TODO fix circle outline not being correct width (should be in pixels)
        // TODO also fix ellipse outline being janky af
        canvash_circle_2D((vec2){0.0f, 0.0f}, 150.0f);
//        canvash_line_2D((vec2){100.0f, -100.0f}, (vec2){100.0f, 100.0f});
//        canvash_ellipse_2D((vec2){0.0f, 0.0f}, 100.0f, 200.0f);
        canvash_no_stroke();

        canvash_reset_transform_2D();

        canvash_fill_color((vec4){0.1f, 0.1f, 0.1f, 1.0f});
        canvash_circle_2D((vec2){0.0f, 0.0f}, 5.0f);
        canvash_circle_2D((vec2){150.0f+15.0f, 0.0f}, 5.0f);
        canvash_circle_2D((vec2){150.0f-15.0f, 0.0f}, 5.0f);

        // NOTE essential (do not leave out)
        canvash_render();
    }

    // NOTE do not forget to terminate, otherwise there will be memory leaks
    canvash_terminate();
    return 0;
}