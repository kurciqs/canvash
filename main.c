#include "canvash_lib/canvash.h"
#define t canvash_time()
#define RAND_COLOR (vec4){(float)(rand()%255)/255.0f,(float)(rand()%255)/255.0f,(float)(rand()%255)/255.0f, 1.0f}
#define RGB_COLOR(x,y,z) (vec4){(float)(x)/255.0f,(float)(y)/255.0f,(float)(z)/255.0f, 1.0f}
#define GRID_AT(i,j) ((i) * grid_size[1] + (j))

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
    glm_vec4_copy(RGB_COLOR(255, 255, 0), colors_by_index[1]);
    glm_vec4_copy(RGB_COLOR(0, 255, 0), colors_by_index[2]);

    int dimensions = 10;
    float width, height;
    canvash_get_window_size(&width, &height);

    ivec2 grid_size = {dimensions, dimensions*height/width};
    vec2 cell_size = {width/grid_size[0], height/grid_size[1]};
    int* grid = (int*)malloc(sizeof(int) * grid_size[0] * grid_size[1]);

    for (int i = 0; i < grid_size[0]; ++i)
    {
        for (int j = 0; j < grid_size[1]; ++j)
        {
            grid[GRID_AT(i,j)] = 0;
        }
    }

    grid[GRID_AT(0,0)] = 1;

    // NOTE this is your main loop, it's like the draw() function in p5.js

    int frame = 0;

    while (canvash_running())
    {
        // NOTE essential (do not leave out)
        canvash_clear_screen();
        canvash_background((vec3){0.2f, 0.3f, canvash_time()/200.0f});

        if (frame >= 60)
        {
            int* new_grid = (int*)malloc(sizeof(int) * grid_size[0] * grid_size[1]);
            memcpy(new_grid, grid, sizeof(int) * grid_size[0] * grid_size[1]);

            for (int i = 0; i < grid_size[0]; ++i)
            {
                for (int j = 0; j < grid_size[1]; ++j)
                {
                    const int id = grid[GRID_AT(i,j)];
                    printf("%d %d %d\n", id, i+1,grid_size[0]);
                    if (id != 0 && i + 1 < grid_size[0] && j + 1 < grid_size[1])
                    {
                        new_grid[GRID_AT(i,j)] = 0;
                        new_grid[GRID_AT(i+1,j)] = 1;
                        new_grid[GRID_AT(i,j+1)] = 1;
                    }
                }
            }

            memcpy(grid, new_grid, sizeof(int) * grid_size[0] * grid_size[1]);
            free((void*)new_grid);

            frame = 0;
        }


        for (int i = 0; i < grid_size[0]; ++i)
        {
            for (int j = 0; j < grid_size[1]; ++j)
            {
                const int id = grid[GRID_AT(i,j)];

                canvash_fill_color(colors_by_index[id]);
                // canvash_fill_color(RGB_COLOR(GRID_AT(i,j) * 10, GRID_AT(i,j) * 10, GRID_AT(i,j) * 10));
                canvash_no_stroke();
                canvash_rectangle_2D((vec2){i*cell_size[0] - width/2.0f, j*cell_size[1] - height/2.0f}, (vec2){(i+1)*cell_size[0]- width/2.0f, (j+1)*cell_size[1] - height/2.0f});
            }
        }

        if (canvash_is_mouse_down(CANVASH_MOUSE_BUTTON_LEFT))
        {
            float cursor_x, cursor_y;
            canvash_get_mouse_position(&cursor_x, &cursor_y);

            float l_width, l_height;
            canvash_get_window_size(&l_width, &l_height);

            float pos_x = cursor_x-(l_width-width)/2.0f;
            float pos_y = (height-cursor_y)+(l_height-height)/2.0f;

            if (!(pos_x < 0.0f || pos_x > width) && !(pos_y < 0.0f || pos_y > height))
            {
                int i, j;
                i = (int)((float)(pos_x)/(cell_size[0]));
                j = (int)((float)(pos_y)/(cell_size[1]));

                const int index = GRID_AT(i,j);
                grid[index] = 1;

                printf("press: %d %d %d\n", i, j, index);
            }
        }

        // NOTE essential (do not leave out)
        canvash_render();
        frame++;
    }

    free((void*)grid);
    // NOTE do not forget to terminate, otherwise there will be serious memory leaks
    canvash_terminate();
    return 0;
}