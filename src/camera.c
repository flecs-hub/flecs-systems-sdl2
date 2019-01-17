#include "sdl_private.h"

void SdlCameraTransform2D(EcsRows *rows) {
    void *row;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsCamera2D *camera = ecs_data(rows, row, 0);

        if (camera->enabled) {
            SdlWindow *wnd = ecs_data(rows, row, 1);
            wnd->projection = wnd->screen;

            ecs_mat3x3_add_translation(&wnd->projection, &camera->position);

            if (camera->zoom.x || camera->zoom.y) {
                ecs_mat3x3_add_scale(&wnd->projection, &camera->zoom);
            }

            break;
        }
    }
}
