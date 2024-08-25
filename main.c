#include "canvash_lib/canvash.h"
#define t canvash_time()
#define RAND_COLOR (vec4){(float)(rand()%255)/255.0f,(float)(rand()%255)/255.0f,(float)(rand()%255)/255.0f, 1.0f}
#define RGB_COLOR(x,y,z) (vec4){(float)(x)/255.0f,(float)(y)/255.0f,(float)(z)/255.0f, 1.0f}


int main()
{
    // NOTE initialize canvash with size 800x600, title "canvash_dev", with an icon, 2D mode (check if it failed) and set the key and mouse callback
    if (!canvash_init("canvash_dev", 800, 600, "res/img/icon.png", twodimensional))
    {
        printf("[USER] failed initialization.\n");
        return -1;
    }

    vec4 colors_by_index[3];

    glm_vec4_copy(RGB_COLOR(0, 0, 0), colors_by_index[0]);
    glm_vec4_copy(RGB_COLOR(255, 0, 0), colors_by_index[1]);
    glm_vec4_copy(RGB_COLOR(0, 255, 0), colors_by_index[2]);

    int dimensions = 40;
    float width, height;
    canvash_get_window_size(&width, &height);

    ivec2 grid_size = {dimensions, dimensions*height/width};
    vec2 cell_size = {width/grid_size[0], height/grid_size[1]};
    int* grid = (int*)malloc(sizeof(int) * grid_size[0] * grid_size[1]);

    for (int i = 0; i < grid_size[0]; ++i)
    {
        for (int j = 0; j < grid_size[1]; ++j)
        {
            grid[i*grid_size[1]+j] = 0;
        }
    }

    // NOTE this is your main loop, it's like the draw() function in p5.js
    while (canvash_running())
    {
        // NOTE essential (do not leave out)
        canvash_clear_screen();
        canvash_background((vec3){0.2f, 0.3f, canvash_time()/200.0f});

        for (int i = 0; i < grid_size[0]; ++i)
        {
            for (int j = 0; j < grid_size[1]; ++j)
            {
                const int id = grid[i*grid_size[1]+j];

                canvash_fill_color(colors_by_index[id]);
                canvash_no_stroke();
                canvash_rectangle_2D((vec2){i*cell_size[0] - width/2.0f, j*cell_size[1] - height/2.0f}, (vec2){(i+1)*cell_size[0]- width/2.0f, (j+1)*cell_size[1] - height/2.0f});
            }
        }

        if (canvash_is_mouse_down(CANVASH_MOUSE_BUTTON_LEFT))
        {
            float cursor_x, cursor_y;
            canvash_get_mouse_position(&cursor_x, &cursor_y);

            int i, j;
            i = (int)((float)(cursor_x)/(cell_size[0]));
            j = (int)((float)(height-cursor_y)/(cell_size[1]));

            const int index = i*grid_size[1]+j;
            grid[index] = 1;
        }

        // NOTE essential (do not leave out)
        canvash_render();
    }

    free((void*)grid);
    // NOTE do not forget to terminate, otherwise there will be serious memory leaks
    canvash_terminate();
    return 0;
}