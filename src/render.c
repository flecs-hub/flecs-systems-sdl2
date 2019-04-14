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
    EcsColor *c,
    EcsLineColor *lc)
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

    if (!c && !lc) {
        c = &WHITE;
    }

    if (c) {
        filledPolygonRGBA(wnd->display, gfx_x, gfx_y, 5, c->r, c->g, c->b, c->a);
    }

    if (lc) {
        polygonRGBA(wnd->display, gfx_x, gfx_y, 5, lc->r, lc->g, lc->b, lc->a);
    }
}

void SdlRenderRectangle(ecs_rows_t *rows) {
    SdlWindow *wnd = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsRectangle *shape = ecs_field(rows, EcsRectangle, i, 1);
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, i, 2);
        EcsColor *c = ecs_field(rows, EcsColor, i, 3);
        EcsLineColor *lc = ecs_field(rows, EcsLineColor, i, 4);
        RenderRectangle(wnd, m, shape->width, shape->height, c, lc);
    }
}

void SdlRenderSquare(ecs_rows_t *rows) {
    SdlWindow *wnd = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsSquare *shape = ecs_field(rows, EcsSquare, i, 1);
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, i, 2);
        EcsColor *c = ecs_field(rows, EcsColor, i, 3);
        EcsLineColor *lc = ecs_field(rows, EcsLineColor, i, 4);
        RenderRectangle(wnd, m, shape->size, shape->size, c, lc);  
    }
}

void SdlRenderCircle(ecs_rows_t *rows) {
    SdlWindow *wnd = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        EcsCircle *shape = ecs_field(rows, EcsCircle, i, 1);
        EcsMatTransform2D *m = ecs_field(rows, EcsMatTransform2D, i, 2);
        EcsColor *c = ecs_field(rows, EcsColor, i, 3);
        EcsLineColor *lc = ecs_field(rows, EcsLineColor, i, 4);

        float radius = shape->radius;

        EcsVec2 position = (EcsVec2){0, 0};
        ecs_mat3x3_transform(m, &position, &position, 1);

        /* Transform position & size to screen coordinates */
        EcsVec2 coord[] = {
            {position.x, position.y},
            {radius, radius}
        };

        ecs_mat3x3_transform(&wnd->projection, coord, coord, 2);

        if (!c && !lc) {
            c = &WHITE;
        }

        if (c) {
            filledEllipseRGBA(wnd->display,
                coord[0].x, coord[0].y, /* position */
                radius * wnd->scale.x, radius * wnd->scale.y, /* size */
                c->r, c->g, c->b, c->a  /* color */
            );
        }

        if (lc) {
            SDL_SetRenderDrawColor(wnd->display, lc->r, lc->g, lc->b, lc->a);

            int steps = radius;
            if (steps > 75) {
                steps = 75;
            }

            SDL_Point *points = alloca(sizeof(SDL_Point) * (steps + 1));
            float step_size = (2.0 * M_PI) / steps;
            int w;
            for (w = 0; w < 2; w ++) {
                int i;
                float x_radius = radius * wnd->scale.x;
                float y_radius = radius * wnd->scale.y;

                for (i = 0; i <= steps; i ++) {
                    points[i].x = cos(step_size * i) * x_radius + coord[0].x;
                    points[i].y = sin(step_size * i) * y_radius + coord[0].y;                
                }
                
                SDL_RenderDrawLines(wnd->display, points, steps + 1);          
                radius -= 0.5;
            }
        }
    }
}

void SdlRender2D(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;
    SdlWindow *wnd = ecs_column(rows, SdlWindow, 1);
    ecs_entity_t SdlRenderSquare = ecs_column_entity(rows, 2);
    ecs_entity_t SdlRenderRectangle = ecs_column_entity(rows, 3);
    ecs_entity_t SdlRenderCircle = ecs_column_entity(rows, 4);

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
