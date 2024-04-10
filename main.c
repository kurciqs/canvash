#include "canvash_lib/canvash.h"

typedef struct {
    vec2 pos;
    vec2 vel;
    vec2 acc;
    float mass;
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

    // TODO boundary collision detection

    glm_vec2_add(mover->vel, mover->acc, mover->vel);
    glm_vec2_add(mover->pos, mover->vel, mover->pos);
    glm_vec2_zero(mover->acc);
}

void draw_mover(Mover* mover) {
//    canvash_no_stroke();
    canvash_stroke_color((vec4){0.0f, 0.0f, 0.0f, 1.0f});
    canvash_stroke(5.0f);
    canvash_fill_color(mover->color);
    canvash_circle_2D(mover->pos, mover->mass);
}

int main() {
    // NOTE initialize canvash with size 800x600, title "canvash_dev", with an icon, 2D mode (check if it failed) and set the key and mouse callback
    if (!canvash_init("canvash_dev", 800, 600, "res/img/icon.png", twodimensional)) {
        printf("[USER] failed initialization.\n");
        return -1;
    }

    int num_movers = 10;
    Mover* movers = (Mover*) malloc(sizeof(Mover) * num_movers);
    for (int i = 0; i < num_movers; i++) {
        Mover* mover = &movers[i];
        vec2 screen_size;
        canvash_get_window_size(&screen_size[0], &screen_size[1]);
        mover->pos[0] = rand() % (int)(screen_size[0]/2.0f) - rand() % (int)(screen_size[0]/2.0f);
        mover->pos[1] = rand() % (int)(screen_size[1]/2.0f) - rand() % (int)(screen_size[1]/2.0f);
        mover->vel[0] = 0.0f;
        mover->vel[1] = 0.0f;
        mover->mass = rand() % 25 + 25;
        glm_vec2_zero(mover->acc);
        mover->color[0] = (float)(rand() % 255) / 255.0f;
        mover->color[1] = (float)(rand() % 255) / 255.0f;
        mover->color[2] = (float)(rand() % 255) / 255.0f;
        mover->color[3] = 1.0f;
    }

    // NOTE this is your main loop, it's like the draw() function in p5.js
    while (canvash_running()) {
        // NOTE essential (do not leave out)
        canvash_clear_screen();
        canvash_background((vec3){0.2f, 0.3f, canvash_time()/200.0f});

        for (int i = 0; i < num_movers; i++) {
            Mover* mover = &movers[i];
            update_mover(mover);
            draw_mover(mover);
        }
        // NOTE essential (do not leave out)
        canvash_render();
    }

    // NOTE do not forget to terminate, otherwise there will be memory leaks
    canvash_terminate();
    return 0;
}