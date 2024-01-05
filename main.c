#include "canvash_lib/canvash.h"


void key_callback(int key, int scancode, int action) {
    if (CANVASH_KEY_IS_PRESSED(key, action, CANVASH_KEY_E)) {
        printf("Hewwo, key callback work!\n");
    }
}

void mouse_callback(int button, int action, float xpos, float ypos) {
    if (CANVASH_BUTTON_IS_PRESSED(button, action, CANVASH_MOUSE_BUTTON_LEFT)) {
        printf("Hewwo, mouse callback work at %f %f!\n", xpos, ypos);
    }
}

int main() {

    if (!canvash_init("canvash_dev", 800, 600, "res/img/icon.png", twodimensional)) {
        printf("[USER] failed initialization.\n");
        return -1;
    }
    canvash_set_key_callback(key_callback);
    canvash_set_mouse_callback(mouse_callback);

    while (!canvash_running()) {
        canvash_clear_screen((vec3){0.2f, 0.3f, 0.3f});

        canvash_rotate_2D(glm_rad(45));
        canvash_rectangle_2D((vec2){0.0f, 0.0f}, (vec2){1.0f, 1.0f}, (vec4){1.0f, 1.0f, 1.0f, 1.0f});
        canvash_rectangle_2D((vec2){0.0f, 0.0f}, (vec2){1.0f, 1.0f}, (vec4){1.0f, 1.0f, 1.0f, 1.0f});
        canvash_rectangle_2D((vec2){0.0f, 0.0f}, (vec2){1.0f, 1.0f}, (vec4){1.0f, 1.0f, 1.0f, 1.0f});

        canvash_render();
    }

    canvash_terminate();
    return 0;

}