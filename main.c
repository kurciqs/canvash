#include "canvash_lib/canvash.h"

typedef struct {
    vec2 pos;
    vec2 vel;
    vec2 acc;
    float mass;
    float energy;
    vec4 color;
    float last_breed;
    bool dead;
} Mover;

void apply_force_mover(Mover* mover, vec2 force) {
    vec2 f;
    glm_vec2_copy(force, f);
    glm_vec2_scale(f, 1.0f/mover->mass, f);
    glm_vec2_add(mover->acc, f, mover->acc);
}

void update_mover(Mover* mover) {
    mover->energy = glm_clamp(mover->energy, 0.0f, 1.0f);
    mover->energy = glm_clamp(mover->energy - 0.001f, 0.0f, 1.0f);
    mover->last_breed += 0.1;
    float theta = glm_rad((float)(rand() % 360));
    float mag = (float)(rand() % 10) / 1.0f;

    vec2 force = {mag * cosf(theta), mag * sinf(theta)};

    apply_force_mover(mover, force);
    glm_vec2_add(mover->vel, mover->acc, mover->vel);

    if (mover->energy > 0.0f) {
        glm_vec2_scale(mover->vel, mover->energy, mover->vel);
        glm_vec2_add(mover->pos, mover->vel, mover->pos);
        glm_vec2_scale(mover->vel, 1.0f / mover->energy, mover->vel);
    }

    glm_vec2_zero(mover->acc);

    glm_vec4_lerp((vec4){0.0f, 1.0f, 0.0f, 1.0f}, (vec4){1.0f, 0.0f, 0.0f, 1.0f}, 1.0f - mover->energy, mover->color);

    vec2 screen_size;
    canvash_get_window_size(&screen_size[0], &screen_size[1]);

    if (mover->pos[0] >= screen_size[0] / 2.0f - mover->mass) {
        mover->pos[0] = screen_size[0] / 2.0f - mover->mass;
        mover->vel[0] *= -1.0f;
    }
    else if (mover->pos[0] <= -screen_size[0] / 2.0f + mover->mass) {
        mover->pos[0] = -screen_size[0] / 2.0f + mover->mass;
        mover->vel[0] *= -1.0f;
    }

    if (mover->pos[1] >= screen_size[1] / 2.0f - mover->mass) {
        mover->pos[1] = screen_size[1] / 2.0f - mover->mass;
        mover->vel[1] *= -1.0f;
    }
    else if (mover->pos[1] <= -screen_size[1] / 2.0f + mover->mass) {
        mover->pos[1] = -screen_size[1] / 2.0f + mover->mass;
        mover->vel[1] *= -1.0f;
    }
}

void draw_mover(Mover* mover) {
    canvash_no_stroke();
    canvash_stroke_color((vec4){0.1f, 0.1f, 0.1f, 1.0f});
//    canvash_stroke(3.0f);
    canvash_fill_color(mover->color);
    canvash_circle_2D(mover->pos, mover->mass);
}

void new_mover(Mover* movers, const int i, const float mass, const vec2 pos, const vec2 vel, const vec4 color) {
    Mover* mover = &movers[i];
    mover->mass = mass;
    mover->pos[0] = pos[0];
    mover->pos[1] = pos[1];
    mover->vel[0] = vel[0];
    mover->vel[1] = vel[1];
    glm_vec2_zero(mover->acc);
    mover->color[0] = color[0];
    mover->color[1] = color[1];
    mover->color[2] = color[2];
    mover->color[3] = color[3];
    mover->energy = 1.0f;
    mover->last_breed = 0.0f;
    mover->dead = false;
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

    int num_movers = 5;
    Mover* movers = (Mover*) malloc(sizeof(Mover) * num_movers);
    vec2 screen_size;
    canvash_get_window_size(&screen_size[0], &screen_size[1]);

    for (int i = 0; i < num_movers; i++) {
        float mass = rand() % 15 + 15;
        vec2 pos;
        pos[0] = rand() % (int)(screen_size[0]/2.0f - (int)mass) - rand() % ((int)(screen_size[0]/2.0f) - (int)mass);
        pos[1] = rand() % (int)(screen_size[1]/2.0f - (int)mass) - rand() % (int)(screen_size[1]/2.0f - (int)mass);
        vec2 vel;
        vel[0] = 0.0f;
        vel[1] = 0.0f;
        new_mover(movers, i, mass, pos, vel, (vec4){0.0f, 1.0f, 0.0f, 1.0f});
    }

    // NOTE this is your main loop, it's like the draw() function in p5.js
    while (canvash_running()) {
        // NOTE essential (do not leave out)
        canvash_clear_screen();
        canvash_background((vec3){0.1f, 0.1f, 0.1f});

        float width, height;
        canvash_get_window_size(&width, &height);
        if (width > 0.0f && height > 0.0f && !canvash_is_button_pressed(CANVASH_MOUSE_BUTTON_RIGHT)) {
            for (int i = 0; i < num_movers; i++) {
                Mover *mover_i = &movers[i];
                for (int j = i; j < num_movers; j++) {
                    if (i == j) continue;
                    Mover *mover_j = &movers[j];
                    float r = glm_vec2_distance(mover_i->pos, mover_j->pos);
                    if (r < mover_i->mass + mover_j->mass && mover_i->last_breed > 5.0f) {
                        if (mover_i->energy == 0.0f || mover_j->energy == 0.0f) continue;
                        // NOTE rule: bigger one wins, no matter what, equal is draw
                        if (mover_i->mass > mover_j->mass) {
                            mover_i->energy += mover_j->energy;
                            mover_j->energy = 0.0f;
                        }
                        else if (mover_i->mass < mover_j->mass) {
                            mover_j->energy += mover_j->energy;
                            mover_i->energy = 0.0f;
                        }
                    }
                }
                // NOTE reproduction
                if (mover_i->energy > 0.5f && rand() % 2 == 0 && mover_i->last_breed > 20.0f) {
                    num_movers++;
                    movers = (Mover*) realloc(movers, num_movers * sizeof(Mover));
                    new_mover(movers, num_movers - 1, glm_clamp(mover_i->mass + (float)(rand() % 15 - 15), 20.0f, 100.0f), mover_i->pos, (vec2){0.0f, 0.0f}, (vec4){0.0f, 1.0f, 0.0f, 1.0f});
                    mover_i->last_breed = 0.0f;
                }
                // NOTE death
                if (mover_i->energy <= 0) {
                    mover_i->dead = true;
                }
            }
            for (int i = 0; i < num_movers; i++)
            {
                Mover *mover_i = &movers[i];
                update_mover(mover_i);
            }
            // NOTE delete from array
            int ind = 0;
            Mover* new_movers = (Mover*) malloc(sizeof(Mover) * num_movers);
            for (int i = 0; i < num_movers; i++) {
                Mover *mover_i = &movers[i];
                update_mover(mover_i);
                if (!mover_i->dead)
                    new_movers[ind++] = *mover_i;
            }
            // NOTE reset the movers array
            free((void*)movers);
            num_movers = ind;
            movers = (Mover*) malloc(sizeof(Mover) * num_movers);
            memcpy(movers, new_movers, sizeof(Mover) * num_movers);
//            printf("%d\n", num_movers);
            free((void*)new_movers);
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
    free((void*)movers);
    return 0;
}