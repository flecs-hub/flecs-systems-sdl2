#include "sdl_private.h"

void SdlCameraTransform2D(ecs_rows_t *rows) {
    int i;
    EcsCamera2D *camera = ecs_column(rows, EcsCamera2D, 1);
    ecs_entity_t root = ecs_column_source(rows, 2);
    ecs_type_t TSdlWindow = ecs_column_type(rows, 3);

    for (i = 0; i < rows->count; i ++) {
        if (camera[i].enabled) {
            SdlWindow *wnd = ecs_get_ptr(rows->world, root, SdlWindow);
            if (!wnd) {
                /* Window creation must have failed */
                break;
            }

            wnd->projection = wnd->screen;

            ecs_mat3x3_add_translation(&wnd->projection, &camera[i].position);

            if (camera[i].zoom.x || camera[i].zoom.y) {
                ecs_mat3x3_add_scale(&wnd->projection, &camera[i].zoom);
            }

            break;
        }
    }
}
