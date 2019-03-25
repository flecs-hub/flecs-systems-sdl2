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

void SdlRenderRectangle(ecs_rows_t *rows) {
    SdlWindow *wnd = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsRectangle *shape = ecs_field(rows, EcsRectangle, i, 1);
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, i, 2);
        EcsColor *c = ecs_field(rows, EcsColor, i, 3);

        if (c) {
            RenderRectangle(wnd, m, shape->width, shape->height, c);
        } else {
            RenderRectangle(wnd, m, shape->width, shape->height, &WHITE);
        }        
    }
}

void SdlRenderSquare(ecs_rows_t *rows) {
    SdlWindow *wnd = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSquare *shape = ecs_field(rows, EcsSquare, i, 1);
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, i, 2);
        EcsColor *c = ecs_field(rows, EcsColor, i, 3);
        if (c) {
            RenderRectangle(wnd, m, shape->size, shape->size, c);
        } else {
            RenderRectangle(wnd, m, shape->size, shape->size, &WHITE);
        }        
    }
}

void SdlRenderCircle(ecs_rows_t *rows) {
    SdlWindow *wnd = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsCircle *shape = ecs_field(rows, EcsCircle, i, 1);
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, i, 2);
        EcsColor *c = ecs_field(rows, EcsColor, i, 3);

        float radius = shape->radius;

        EcsVec2 position = (EcsVec2){0, 0};
        ecs_mat3x3_transform(m, &position, &position, 1);

        /* Transform position & size to screen coordinates */
        EcsVec2 coord[] = {
            {position.x, position.y},
            {radius, radius}
        };

        ecs_mat3x3_transform(&wnd->projection, coord, coord, 2);

        if (c) {
            filledEllipseRGBA(wnd->display,
                coord[0].x, coord[0].y, /* position */
                radius * wnd->scale.x, radius * wnd->scale.y, /* size */
                c->r, c->g, c->b, c->a  /* color */
            );
        } else {
            filledEllipseRGBA(wnd->display,
                coord[0].x, coord[0].y, /* position */
                radius * wnd->scale.x, radius * wnd->scale.y, /* size */
                WHITE.r, WHITE.g, WHITE.b, WHITE.a  /* color */
            );            
        }
    }
}

void SdlRender2D(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    SdlWindow *wnd = ecs_column(rows, SdlWindow, 1);
    EcsEntity SdlRenderSquare = ecs_column_component(rows, 2);
    EcsEntity SdlRenderRectangle = ecs_column_component(rows, 3);
    EcsEntity SdlRenderCircle = ecs_column_component(rows, 4);

    int i;
    for (i = 0; i < rows->count; i ++) {
        SDL_SetRenderDrawColor(wnd[i].display, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(wnd[i].display);

        ecs_run(world, SdlRenderSquare, 0, wnd);
        ecs_run(world, SdlRenderRectangle, 0, wnd);
        ecs_run(world, SdlRenderCircle, 0, wnd);

        SDL_RenderPresent(wnd[i].display);
    }
}
