#include <include/sdl2.h>
#include <SDL2/SDL.h>
#include "SDL2_gfxPrimitives.h"

typedef struct Sdl2Window {
    SDL_Window *window;
    SDL_Renderer *display;
} Sdl2Window;

static
void Vec2IntToGfx(
    EcsVec2Int *v,
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
void DrawRectangle(
    Sdl2Window *wnd,
    EcsPosition2D *p,
    EcsRotation2D*r,
    EcsScale2D *s,
    EcsRectangle *rectangle)
{
    int w = rectangle->width / 2.0;
    int h = rectangle->height / 2.0;

    SDL_Rect rect = {
        .x = p->x - w,
        .y = p->y - h,
        .w = rectangle->width,
        .h = rectangle->height
    };

    if (!r) {
        SDL_RenderFillRect(wnd->display, &rect);
    } else {
        EcsVec2Int points[5];

        points[0].x = -w;
        points[0].y = -h;
        points[1].x = w;
        points[1].y = -h;
        points[2].x = w;
        points[2].y = h;
        points[3].x = -w;
        points[3].y = h;
        points[4].x = -w;
        points[4].y = -h;

        EcsMat3x3 transform = ECS_MAT3X3_IDENTITY;
        if (r) ecs_mat3x3_add_rotation(&transform, r->angle);
        if (p) ecs_mat3x3_add_translation(&transform, p);
        ecs_mat3x3_transform_int(&transform, points, points, 5);

        Sint16 gfx_x[5];
        Sint16 gfx_y[5];
        Vec2IntToGfx(points, gfx_x, gfx_y, 5);

        filledPolygonRGBA(wnd->display, gfx_x, gfx_y, 5, 255, 255, 255, 255);
    }
}

static
void DrawSquare(
    Sdl2Window *wnd,
    EcsPosition2D *p,
    EcsRotation2D*r,
    EcsScale2D *s,
    EcsSquare *square)
{
    EcsRectangle rect = {
        .width = square->size,
        .height = square->size
    };

    DrawRectangle(wnd, p, r, s, &rect);
}

static
void DrawCircle(
    Sdl2Window *wnd,
    EcsPosition2D *p,
    EcsScale2D *s,
    EcsCircle *circle)
{
    float x = p->x;
    float y = p->y;
    float radius = circle->radius;

    float dy;
    for (dy = 1.0; dy <= radius; dy += 1.0)
    {
        double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));

        SDL_RenderDrawLine(wnd->display,
            x - dx, y + dy - radius, x + dx, y + dy - radius);

        SDL_RenderDrawLine(wnd->display,
            x - dx, y - dy + radius, x + dx, y - dy + radius);
    }
}

static
void SdlDraw2DShapes(
    EcsRows *rows)
{
    void *row;

    EcsHandle shape_h = ecs_handle(rows, 5);
    EcsHandle EcsSquare_h = ecs_handle(rows, 6);
    EcsHandle EcsRectangle_h = ecs_handle(rows, 7);
    EcsHandle EcsCircle_h = ecs_handle(rows, 8);
    EcsHandle EcsTriangle_h = ecs_handle(rows, 9);

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Sdl2Window *wnd = ecs_column(rows, row, 0);
        EcsPosition2D *p = ecs_column(rows, row, 1);
        EcsRotation2D *r = ecs_column(rows, row, 2);
        EcsScale2D *s = ecs_column(rows, row, 3);
        EcsColor *c = ecs_column(rows, row, 4);
        void *shape = ecs_column(rows, row, 5);

        if (c) {
            SDL_SetRenderDrawColor(wnd->display, c->r, c->g, c->b, c->a);
        } else {
            SDL_SetRenderDrawColor(wnd->display, 255, 255, 255, 255);
        }

        if (shape_h == EcsSquare_h) {
            DrawSquare(wnd, p, r, s, shape);
        } else if (shape_h == EcsRectangle_h) {
            DrawRectangle(wnd, p, r, s, shape);
        } else if (shape_h == EcsCircle_h) {
            DrawCircle(wnd, p, s, shape);
        }
    }
}

static
void PollWindow(
    EcsWorld *world,
    Sdl2Window *wnd,
    EcsHandle SdlDraw2DShapes_h)
{
    SDL_Event e;
    SDL_SetRenderDrawColor(wnd->display, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(wnd->display);

    ecs_run_system(world, SdlDraw2DShapes_h, 0, 0, NULL);

    SDL_RenderPresent(wnd->display);
    SDL_PollEvent(&e);

    if (e.type == SDL_QUIT) {
        ecs_quit(world);
    }
}

static
void SdlPoll(EcsRows *rows) {
    void *row;
    EcsHandle SdlDraw2DShapes_h = ecs_handle(rows, 1);
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Sdl2Window *wnd = ecs_column(rows, row, 0);
        PollWindow(rows->world, wnd, SdlDraw2DShapes_h);
    }
}

static
void SdlInitWindow(EcsRows *rows) {
    EcsWorld *world = rows->world;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsCanvas2D *canvas = ecs_column(rows, row, 0);
        EcsHandle Sdl2Window_h = ecs_handle(rows, 1);
        EcsHandle entity = ecs_entity(row);

        SDL_Window *window = SDL_CreateWindow(
            "SDL2 Window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            canvas->window.width,
            canvas->window.height,
            SDL_WINDOW_OPENGL
        );

        if (!window) {
            fprintf(stderr, "SDL2 window creation failed for canvas: %s\n",
                SDL_GetError());
            continue;
        }

        SDL_Renderer *display = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

        if (!display) {
            fprintf(stderr, "SDL2 renderer creation failed for canvas: %s\n",
              SDL_GetError());
            continue;
        }

        ecs_add(world, entity, EcsComponent_h);

        ecs_set(world, entity, Sdl2Window, {
          .window = window,
          .display = display
        });
    }
}

static
void SdlDeinitWindow(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Sdl2Window *window = ecs_column(rows, row, 0);
        SDL_DestroyWindow(window->window);
    }
}

static
void SdlDeinit(EcsRows *rows) {
    SDL_Quit();
}

void EcsSystemsSdl2(
    EcsWorld *world,
    int flags,
    void *handles_out)
{
    /*bool do_2d = !flags || flags & ECS_2D;
    bool do_3d = !flags || flags & ECS_3D;*/

    EcsSystemsSdl2Handles *handles = handles_out;

    ECS_IMPORT(world, EcsComponentsGraphics, flags);
    ECS_IMPORT(world, EcsComponentsGeometry, flags);
    ECS_IMPORT(world, EcsComponentsTransform, flags);

    ECS_COMPONENT(world, Sdl2Window);

    ECS_SYSTEM(world, SdlInitWindow, EcsOnSet, EcsCanvas2D, HANDLE.Sdl2Window);
    ECS_SYSTEM(world, SdlDeinitWindow, EcsOnRemove, Sdl2Window);
    ECS_SYSTEM(world, SdlDeinit, EcsOnRemove, 0);
    ECS_SYSTEM(world, SdlDraw2DShapes, EcsOnDemand, COMPONENT.Sdl2Window,
        EcsPosition2D, ?EcsRotation2D, ?EcsScale2D, ?EcsColor,
        EcsSquare | EcsRectangle | EcsCircle | EcsTriangle,
        HANDLE.EcsSquare, HANDLE.EcsRectangle, HANDLE.EcsCircle, HANDLE.EcsTriangle);
    ECS_SYSTEM(world, SdlPoll, EcsOnFrame, Sdl2Window, HANDLE.SdlDraw2DShapes);

    ECS_FAMILY(world, Sdl2, SdlInitWindow, SdlDeinitWindow, SdlPoll);

    ecs_add(world, SdlInitWindow_h, EcsHidden_h);
    ecs_add(world, SdlDeinitWindow_h, EcsHidden_h);
    ecs_add(world, SdlPoll_h, EcsHidden_h);
    ecs_commit(world, SdlInitWindow_h);
    ecs_commit(world, SdlDeinitWindow_h);
    ecs_commit(world, SdlPoll_h);

    SDL_Init(SDL_INIT_VIDEO | SDL_WINDOW_OPENGL);

    handles->Sdl2 = Sdl2_h;
}
