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
    vec2 g = {0.0f, -9.8f};
    apply_force_mover(mover, g);

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

    glm_vec2_scale(mover->vel, 0.9f, mover->vel);
}

void draw_mover(Mover* mover) {
    canvash_no_stroke();
    canvash_stroke_color((vec4){0.1f, 0.1f, 0.1f, 1.0f});
//    canvash_stroke(3.0f);
    canvash_fill_color(mover->color);
    canvash_circle_2D(mover->pos, mover->radius);
}

void new_mover(Mover* movers, const int i, const float mass, const float radius, const vec2 pos, const vec2 vel, const vec4 color) {
    Mover* mover = &movers[i];
    mover->mass = mass;
    mover->radius = radius;
    mover->pos[0] = pos[0];
    mover->pos[1] = pos[1];
    mover->vel[0] = vel[0];
    mover->vel[1] = vel[1];
    glm_vec2_zero(mover->acc);
    mover->color[0] = color[0];
    mover->color[1] = color[1];
    mover->color[2] = color[2];
    mover->color[3] = color[3];
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

    int num_movers = 100;
    Mover* movers = (Mover*) malloc(sizeof(Mover) * num_movers);
    vec2 screen_size;
    canvash_get_window_size(&screen_size[0], &screen_size[1]);

    for (int i = 0; i < num_movers; i++) {
        float mass = 15.0f;
        float radius = mass;
        vec2 pos;
        pos[0] = rand() % (int)(screen_size[0]/2.0f - (int)radius) - rand() % ((int)(screen_size[0]/2.0f) - (int)radius);
        pos[1] = rand() % (int)(screen_size[1]/2.0f - (int)radius) - rand() % (int)(screen_size[1]/2.0f - (int)radius);
        vec2 vel;
        vel[0] = 0.0f;
        vel[1] = 0.0f;
        new_mover(movers, i, radius, mass, pos, vel, CANVASH_RANDOM_COLOR);
    }
//    new_mover(movers, num_movers - 1, 500.0f, 25.0f, (vec2){0.0f, 0.0f}, (vec2){0.0f, 0.0f}, (vec4){0.0f, 0.0f, 0.0f, 1.0f});

    // NOTE this is your main loop, it's like the draw() function in p5.js
    while (canvash_running()) {
        // NOTE essential (do not leave out)
        canvash_clear_screen();
        canvash_background((vec3){0.1f, 0.1f, 0.1f});

        if (!canvash_is_button_pressed(CANVASH_MOUSE_BUTTON_RIGHT)) {
            for (int i = 0; i < num_movers; i++) {
                Mover *mover_i = &movers[i];

                for (int j = 0; j < num_movers; j++) {
                    if (i == j) continue;
                    Mover *mover_j = &movers[j];
                    vec2 force = {0.0f, 0.0f};
                    float G = 10e-0f;
                    vec2 r_vec;
                    glm_vec2_sub(mover_j->pos, mover_i->pos, r_vec);
                    float r = glm_vec2_norm(r_vec);
                    glm_vec2_normalize(r_vec);
                    glm_vec2_copy(r_vec, force);
                    if (r < 25.0f) r = 25.0f; // clamp r to avoid explosion
                    glm_vec2_scale(force, G * mover_i->mass * mover_j->mass * (1.0f / (r * r)), force);
                    apply_force_mover(mover_i, force);
                }
                update_mover(mover_i);
            }

            // NOTE collision detection (has priority)
            /*
            for (int i = 0; i < num_movers; i++) {
                Mover *mover_i = &movers[i];
                for (int j = i; j < num_movers; j++) {
                    if (i == j) continue;
                    Mover *mover_j = &movers[j];
                    vec2 r_vec;
                    glm_vec2_sub(mover_j->pos, mover_i->pos, r_vec);
                    float r = glm_vec2_norm(r_vec);
                    glm_vec2_normalize(r_vec);
                    if (r < mover_i->radius + mover_j->radius) {
                        vec2 v1;
                        vec2 v2;
                        vec2 u1;
                        vec2 u2;
                        glm_vec2_copy(mover_i->vel, u1);
                        glm_vec2_scale(u1, (mover_i->mass - mover_j->mass) / (mover_i->mass + mover_j->mass), v1);
                        glm_vec2_scale(u1, (2.0f * mover_i->mass) / (mover_i->mass + mover_j->mass), v2);
                        glm_vec2_copy(mover_j->vel, u2);
                        glm_vec2_scale(u2, (2.0f * mover_j->mass) / (mover_i->mass + mover_j->mass), u2);
                        glm_vec2_add(u2, v1, v1);
                        glm_vec2_copy(mover_j->vel, u2);
                        glm_vec2_scale(u2, (mover_j->mass - mover_i->mass) / (mover_i->mass + mover_j->mass), u2);
                        glm_vec2_add(u2, v2, v2);

                        glm_vec2_copy(v1, mover_i->vel);
                        glm_vec2_copy(v2, mover_j->vel);

                        glm_vec2_scale(mover_i->vel, 0.9f, mover_i->vel);
                        glm_vec2_scale(mover_j->vel, 0.9f, mover_j->vel);

                        glm_vec2_copy(mover_j->pos, mover_i->pos);
                        glm_vec2_scale(r_vec, mover_i->radius + mover_j->radius, r_vec);
                        glm_vec2_sub(mover_i->pos, r_vec, mover_i->pos);
                    }
                }
            }*/
        }

        for (int i = 0; i < num_movers; i++) {
            Mover *mover_i = &movers[i];
            draw_mover(mover_i);
        }

        // NOTE essential (do not leave out)
        canvash_render();
    }

    // NOTE do not forget to terminate, otherwise there will be memory leaks
    canvash_terminate();
    free(movers);
    return 0;
}