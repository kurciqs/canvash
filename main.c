#include "canvash_lib/canvash.h"

typedef struct {
    vec2 pos;
    vec2 vel;
    vec2 acc;
    float mass;
    float radius;
    vec4 color;
} Mover;

void apply_force_mover(Mover* mover, vec2 force) {
    vec2 f;
    glm_vec2_copy(force, f);
    glm_vec2_scale(f, 1.0f/mover->mass, f);
    glm_vec2_add(mover->acc, f, mover->acc);
}

void update_mover(Mover* mover) {
    // NOTE gravity
//    vec2 g = {0.0f, -9.8f};
//    apply_force_mover(mover, g);

    // TODO boundary collision detection

    glm_vec2_add(mover->vel, mover->acc, mover->vel);
    glm_vec2_add(mover->pos, mover->vel, mover->pos);
    glm_vec2_zero(mover->acc);

    vec2 screen_size;
    canvash_get_window_size(&screen_size[0], &screen_size[1]);

    if (mover->pos[0] >= screen_size[0] / 2.0f - mover->radius) {
        mover->pos[0] = screen_size[0] / 2.0f - mover->radius;
        mover->vel[0] *= -1.0f;
    }
    else if (mover->pos[0] <= -screen_size[0] / 2.0f + mover->radius) {
        mover->pos[0] = -screen_size[0] / 2.0f + mover->radius;
        mover->vel[0] *= -1.0f;
    }

    if (mover->pos[1] >= screen_size[1] / 2.0f - mover->radius) {
        mover->pos[1] = screen_size[1] / 2.0f - mover->radius;
        mover->vel[1] *= -1.0f;
    }
    else if (mover->pos[1] <= -screen_size[1] / 2.0f + mover->radius) {
        mover->pos[1] = -screen_size[1] / 2.0f + mover->radius;
        mover->vel[1] *= -1.0f;
    }

//    glm_vec2_scale(mover->vel, 0.98f, mover->vel);
}

void draw_mover(Mover* mover) {
    canvash_no_stroke();
    canvash_stroke_color((vec4){0.1f, 0.1f, 0.1f, 1.0f});
    canvash_stroke(3.0f);
    canvash_fill_color(mover->color);
    canvash_circle_2D(mover->pos, mover->radius);
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
    canvash_set_mouse_callback(mouse_callback);

    int num_movers = 2;
    Mover* movers = (Mover*) malloc(sizeof(Mover) * num_movers);
    vec2 screen_size;
    canvash_get_window_size(&screen_size[0], &screen_size[1]);

//    for (int i = 0; i < num_movers - 1; i++) {
//        Mover* mover = &movers[i];
//        mover->mass = rand() % 25 + 25;
//        mover->radius = mover->mass;
//        mover->pos[0] = rand() % (int)(screen_size[0]/2.0f - (int)mover->mass) - rand() % ((int)(screen_size[0]/2.0f) - (int)mover->mass);
//        mover->pos[1] = rand() % (int)(screen_size[1]/2.0f - (int)mover->mass) - rand() % (int)(screen_size[1]/2.0f - (int)mover->mass);
//        mover->vel[0] = rand() % 25;
//        mover->vel[1] = rand() % 25;
//        glm_vec2_zero(mover->acc);
//        mover->color[0] = (float)(rand() % 255) / 255.0f;
//        mover->color[1] = (float)(rand() % 255) / 255.0f;
//        mover->color[2] = (float)(rand() % 255) / 255.0f;
//        mover->color[3] = 1.0f;
//    }

    {
        Mover* mover = &movers[num_movers - 2];
        mover->mass = 25.0f;
        mover->radius = 25.0f;
        mover->pos[0] = 200.0f;
        mover->pos[1] = 300.0f;
        mover->vel[0] = -20.0f;
        mover->vel[1] = 0.0f;
        glm_vec2_zero(mover->acc);
        mover->color[0] = 1.0f;
        mover->color[1] = 0.0f;
        mover->color[2] = 0.0f;
        mover->color[3] = 1.0f;
    }

    {
        Mover* mover = &movers[num_movers - 1];
        mover->mass = 50000.0f;
        mover->radius = 25.0f;
        mover->pos[0] = 0.0f;
        mover->pos[1] = 0.0f;
        mover->vel[0] = 0.0f;
        mover->vel[1] = 0.0f;
        glm_vec2_zero(mover->acc);
        mover->color[0] = 0.0f;
        mover->color[1] = 0.0f;
        mover->color[2] = 0.0f;
        mover->color[3] = 1.0f;
    }

    // NOTE this is your main loop, it's like the draw() function in p5.js
    while (canvash_running()) {
        // NOTE essential (do not leave out)
        canvash_clear_screen();
        canvash_background((vec3){0.1f, 0.1f, 0.1f});

        for (int i = 0; i < num_movers; i++) {
            Mover* mover_i = &movers[i];
            if (mover_i->mass == mover_i->radius) {
                if (canvash_is_button_pressed(CANVASH_MOUSE_BUTTON_LEFT)) {
                    vec2 mouse_pos;
                    vec2 window_size;
                    canvash_get_mouse_position(&mouse_pos[0], &mouse_pos[1]);
                    canvash_get_window_size(&window_size[0], &window_size[1]);
                    glm_vec2_scale(window_size, 0.5f, window_size);
                    glm_vec2_sub(mouse_pos, window_size, mouse_pos);
                    mouse_pos[1] *= -1.0f;
                    glm_vec2_copy(mouse_pos, mover_i->pos);
                    glm_vec2_copy((vec2){-10.0f, 0.0f}, mover_i->vel);
                }
            }
            for (int j = 0; j < num_movers; j++) {
                if (i == j) continue;
                Mover* mover_j = &movers[j];
                vec2 force = {0.0f, 0.0f};
                float G = 10e-1f;
                vec2 r_vec;
                glm_vec2_sub(mover_j->pos, mover_i->pos, r_vec);
                float r = glm_vec2_norm(r_vec);
                glm_vec2_normalize(r_vec);
                glm_vec2_copy(r_vec, force);
                if (r < 25.0f) r = 25.0f; // clamp r to avoid explosion
                glm_vec2_scale(force, G * mover_i->mass * mover_j->mass * (1.0f / (r*r)), force);
                apply_force_mover(mover_i, force);
            }
            update_mover(mover_i);
            draw_mover(mover_i);
        }
        // NOTE essential (do not leave out)
        canvash_render();
    }

    // NOTE do not forget to terminate, otherwise there will be memory leaks
    canvash_terminate();
    return 0;
}