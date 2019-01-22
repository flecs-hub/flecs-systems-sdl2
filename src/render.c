#include "sdl_private.h"

static EcsColor WHITE = {255, 255, 255, 255};

static
void Vec2ToGfx(
    EcsVec2 *v,
    Sint16 *x_vec_out,
    Sint16 *y_vec_out,
    uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i ++) {
        x_vec_out[i] = v[i].x;
        y_vec_out[i] = v[i].y;
    }
}

static
void RenderRectangle(
    SdlWindow *wnd,
    EcsMatTransform2D *m,
    uint32_t width,
    uint32_t height,
    EcsColor *c)
{
    int w = width / 2.0;
    int h = height / 2.0;

    EcsVec2 points[5];
    points[0] = (EcsVec2){-w, -h};
    points[1] = (EcsVec2){w, -h};
    points[2] = (EcsVec2){w, h};
    points[3] = (EcsVec2){-w, h};
    points[4] = (EcsVec2){-w, -h};

    ecs_mat3x3_transform(m, points, points, 5);
    ecs_mat3x3_transform(&wnd->projection, points, points, 5);

    Sint16 gfx_x[5];
    Sint16 gfx_y[5];
    Vec2ToGfx(points, gfx_x, gfx_y, 5);

    filledPolygonRGBA(wnd->display, gfx_x, gfx_y, 5, c->r, c->g, c->b, c->a);
}

void SdlRenderRectangle(EcsRows *rows) {
    void *row;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        SdlWindow *wnd = ecs_data(rows, row, 0);
        EcsRectangle *shape = ecs_data(rows, row, 1);
        EcsMatTransform2D *m = ecs_data(rows, row, 2);
        EcsColor *c = ecs_data(rows, row, 3);

        if (!c) {
            c = &WHITE;
        }

        RenderRectangle(wnd, m, shape->width, shape->height, c);
    }
}

void SdlRenderSquare(EcsRows *rows) {
    void *row;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        SdlWindow *wnd = ecs_data(rows, row, 0);
        EcsSquare *shape = ecs_data(rows, row, 1);
        EcsMatTransform2D *m = ecs_data(rows, row, 2);
        EcsColor *c = ecs_data(rows, row, 3);

        if (!c) {
            c = &WHITE;
        }

        RenderRectangle(wnd, m, shape->size, shape->size, c);
    }
}

void SdlRenderCircle(EcsRows *rows) {
    void *row;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        SdlWindow *wnd = ecs_data(rows, row, 0);
        EcsCircle *shape = ecs_data(rows, row, 1);
        EcsMatTransform2D *m = ecs_data(rows, row, 2);
        EcsColor *c = ecs_data(rows, row, 3);

        if (!c) {
            c = &WHITE;
        }

        float radius = shape->radius;

        EcsVec2 position = (EcsVec2){0, 0};
        ecs_mat3x3_transform(m, &position, &position, 1);

        /* Transform position & size to screen coordinates */
        EcsVec2 coord[] = {
            {position.x, position.y},
            {radius, radius}
        };

        ecs_mat3x3_transform(&wnd->projection, coord, coord, 2);

        filledEllipseRGBA(wnd->display,
            coord[0].x, coord[0].y, /* position */
            radius * wnd->scale.x, radius * wnd->scale.y, /* size */
            c->r, c->g, c->b, c->a  /* color */
        );
    }
}

void SdlRender2D(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity SdlRenderSquare_h = ecs_component(rows, 1);
    EcsEntity SdlRenderRectangle_h = ecs_component(rows, 2);
    EcsEntity SdlRenderCircle_h = ecs_component(rows, 3);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity root = ecs_entity(rows, row, ECS_ROW_ENTITY);
        SdlWindow *wnd = ecs_data(rows, row, 0);

        SDL_SetRenderDrawColor(wnd->display, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(wnd->display);

        ecs_run_w_filter(world, SdlRenderSquare_h, 0, 0, 0, root, NULL);
        ecs_run_w_filter(world, SdlRenderRectangle_h, 0, 0, 0, root, NULL);
        ecs_run_w_filter(world, SdlRenderCircle_h, 0, 0, 0, root, NULL);

        SDL_RenderPresent(wnd->display);
    }
}
