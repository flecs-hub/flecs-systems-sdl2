#include "sdl_private.h"

void SdlCameraTransform2D(EcsRows *rows) {
    void *row;
    EcsEntity SdlWindow_h = ecs_component(rows, 2);

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsCamera2D *camera = ecs_data(rows, row, 0);

        if (camera->enabled) {
            EcsEntity root = ecs_entity(rows, row, 1);
            SdlWindow *wnd = ecs_get_ptr(rows->world, root, SdlWindow_h);
            wnd->projection = wnd->screen;

            ecs_mat3x3_add_translation(&wnd->projection, &camera->position);

            if (camera->zoom.x || camera->zoom.y) {
                ecs_mat3x3_add_scale(&wnd->projection, &camera->zoom);
            }

            break;
        }
    }
}
