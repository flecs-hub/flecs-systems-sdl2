#include <include/sdl2.h>
#include <SDL2/SDL.h>
#include "SDL2_gfxPrimitives.h"

typedef struct Sdl2Window {
    SDL_Window *window;
    SDL_Renderer *display;
    EcsMat3x3 projection;
    EcsVec2 scale; /* when only scaling is required */
} Sdl2Window;

static
void Vec2ToGfx(
    EcsCanvas2D *canvas,
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
void DrawRectangle(
    Sdl2Window *wnd,
    EcsCanvas2D *canvas,
    EcsMat3x3 *transform,
    EcsPosition2D *p,
    EcsRotation2D*r,
    EcsScale2D *s,
    EcsColor *c,
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
        EcsVec2 points[5];
        points[0] = (EcsVec2){-w, -h};
        points[1] = (EcsVec2){w, -h};
        points[2] = (EcsVec2){w, h};
        points[3] = (EcsVec2){-w, h};
        points[4] = (EcsVec2){-w, -h};

        if (r) ecs_mat3x3_add_rotation(transform, r->angle);
        if (p) ecs_mat3x3_add_translation(transform, p);
        ecs_mat3x3_transform(transform, points, points, 5);

        ecs_mat3x3_transform(&wnd->projection, points, points, 5);

        Sint16 gfx_x[5];
        Sint16 gfx_y[5];
        Vec2ToGfx(canvas, points, gfx_x, gfx_y, 5);

        filledPolygonRGBA(wnd->display, gfx_x, gfx_y, 5, c->r, c->g, c->b, c->a);
    }
}

static
void DrawSquare(
    Sdl2Window *wnd,
    EcsCanvas2D *canvas,
    EcsMat3x3 *transform,
    EcsPosition2D *p,
    EcsRotation2D*r,
    EcsScale2D *s,
    EcsColor *c,
    EcsSquare *square)
{
    EcsRectangle rect = {
        .width = square->size,
        .height = square->size
    };

    DrawRectangle(wnd, canvas, transform, p, r, s, c, &rect);
}

static
void DrawCircle(
    Sdl2Window *wnd,
    EcsCanvas2D *canvas,
    EcsMat3x3 *transform,
    EcsPosition2D *p,
    EcsScale2D *s,
    EcsColor *c,
    EcsCircle *circle)
{
    float radius = circle->radius;

    EcsVec2 position = (EcsVec2){0, 0};
    if (p) ecs_mat3x3_add_translation(transform, p);
    ecs_mat3x3_transform(transform, &position, &position, 1);

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

typedef struct SdlDraw2DShapesParam {
    EcsMat3x3 *transform;
    Sdl2Window *wnd;
    EcsCanvas2D *canvas;
} SdlDraw2DShapesParam;

static
void SdlDraw2DShapesIntern(
    Sdl2Window *wnd,
    EcsCanvas2D *canvas,
    EcsMat3x3 *transform,
    EcsRows *rows,
    EcsHandle DrawChildren_h)
{
    void *row;
    EcsColor white = {.r = 255, .g = 255, .b = 255, .a = 255};

    EcsHandle shape_h = ecs_handle(rows, 4);
    EcsHandle EcsSquare_h = ecs_handle(rows, 5);
    EcsHandle EcsRectangle_h = ecs_handle(rows, 6);
    EcsHandle EcsCircle_h = ecs_handle(rows, 7);
    EcsHandle EcsTriangle_h = ecs_handle(rows, 8);

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsPosition2D *p = ecs_column(rows, row, 0);
        EcsRotation2D *r = ecs_column(rows, row, 1);
        EcsScale2D *s = ecs_column(rows, row, 2);
        EcsColor *c = ecs_column(rows, row, 3);
        void *shape = ecs_column(rows, row, 4);
        void *is_container = ecs_column(rows, row, 9);

        if (!c) {
            c = &white;
        }

        /* Reset matrix for every shape */
        EcsMat3x3 local = *transform;

        if (shape_h == EcsSquare_h) {
            DrawSquare(wnd, canvas, &local, p, r, s, c, shape);
        } else if (shape_h == EcsRectangle_h) {
            DrawRectangle(wnd, canvas, &local, p, r, s, c, shape);
        } else if (shape_h == EcsCircle_h) {
            DrawCircle(wnd, canvas, &local, p, s, c, shape);
        }

        if (is_container) {
            EcsHandle entity = ecs_entity(row);
            SdlDraw2DShapesParam param = {
                .transform = &local, .wnd = wnd, .canvas = canvas
            };
            ecs_run_system(
                rows->world, DrawChildren_h, rows->delta_time,
                entity, &param);
        }
    }
}

static
void SdlDraw2DShapesChildren(
    EcsRows *rows)
{
    SdlDraw2DShapesParam *param = rows->param;
    SdlDraw2DShapesIntern(param->wnd,
        param->canvas, param->transform, rows, rows->system);
}

static
void SdlDraw2DShapes(
    EcsRows *rows)
{
    Sdl2Window *wnd = ecs_column(rows, NULL, 10);
    EcsMat3x3 transform = ECS_MAT3X3_IDENTITY;
    EcsCanvas2D *canvas = ecs_column(rows, NULL, 11);
    EcsHandle DrawChildren_h = ecs_handle(rows, 12);
    SdlDraw2DShapesIntern(wnd, canvas, &transform, rows, DrawChildren_h);
}

static
void Sdl2Render(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsHandle SdlDraw2DShapes_h = ecs_handle(rows, 1);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Sdl2Window *wnd = ecs_column(rows, row, 0);
        SDL_SetRenderDrawColor(wnd->display, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(wnd->display);
        ecs_run_system(world, SdlDraw2DShapes_h, 0, 0, NULL);
        SDL_RenderPresent(wnd->display);
    }
}

static
void Sdl2Input(EcsRows *rows) {
    EcsWorld *world = rows->world;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsInput *input = ecs_column(rows, row, 0);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                ecs_quit(world);

            } else if (e.type == SDL_KEYDOWN) {
                uint32_t sym = e.key.keysym.sym;
                if (sym < 128) {
                    input->keys[sym].pressed = true;
                    input->keys[sym].t_down = e.key.timestamp;
                }

            } else if (e.type == SDL_KEYUP) {
                uint32_t sym = e.key.keysym.sym;
                if (sym < 128) {
                    input->keys[sym].pressed = false;
                    input->keys[sym].t_up = e.key.timestamp;
                }

            } else if (e.type == SDL_WINDOWEVENT) {
                switch (e.window.event) {
                case SDL_WINDOWEVENT_SHOWN:
                    /*SDL_Log("Window %d shown", e.window.windowID);*/
                    break;
                case SDL_WINDOWEVENT_HIDDEN:
                    /*SDL_Log("Window %d hidden", e.window.windowID);*/
                    break;
                case SDL_WINDOWEVENT_EXPOSED:
                    /*SDL_Log("Window %d exposed", e.window.windowID);*/
                    break;
                case SDL_WINDOWEVENT_MOVED:
                    /*SDL_Log("Window %d moved to %d,%d",
                            e.window.windowID, e.window.data1,
                            e.window.data2);*/
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    /*SDL_Log("Window %d resized to %dx%d",
                            e.window.windowID, e.window.data1,
                            e.window.data2);*/
                    break;
                }
            }
        }
    }
}

static
void SdlInitWindow(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsHandle Sdl2Window_h = ecs_handle(rows, 1);
    EcsHandle EcsInput_h = ecs_handle(rows, 2);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsCanvas2D *canvas = ecs_column(rows, row, 0);
        EcsHandle entity = ecs_entity(row);

        /* Create SDL Window with support for OpenGL and high resolutions */
        SDL_Window *window = SDL_CreateWindow(
            "SDL2 Window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            canvas->window.width,
            canvas->window.height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
        );

        if (!window) {
            fprintf(stderr, "SDL2 window creation failed for canvas: %s\n",
                SDL_GetError());
            continue;
        }

        if (!canvas->viewport.width) {
            canvas->viewport.width = canvas->window.width;
            canvas->viewport.height = canvas->window.height;
        }

        /* Set actual dimensions, as this may be a high resolution display */
        int32_t w, h;
        SDL_GL_GetDrawableSize(window, &w, &h);
        canvas->window = (EcsRect){.width = w, .height = h};


        /* Create SDL renderer */
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


        /* Prepare projection matrix */
        EcsMat3x3 projection = ECS_MAT3X3_IDENTITY;
        EcsVec2 scale = {
            (float)canvas->window.width / (float)canvas->viewport.width,
            (float)canvas->window.height / (float)canvas->viewport.height
        };
        EcsVec2 translate = {
            canvas->window.width / 2,
            canvas->window.height / 2
        };

        ecs_mat3x3_add_scale(&projection, &scale);
        ecs_mat3x3_add_translation(&projection, &translate);

        /* Add Sdl2Window component */
        ecs_set(world, entity, Sdl2Window, {
            .window = window,
            .display = display,
            .projection = projection,
            .scale = scale
        });


        /* Add EcsInput component */
        ecs_add(world, entity, EcsInput_h);

        /* Allow canvas to be used as container for scene objects */
        ecs_add(world, entity, EcsContainer_h);

        /* Commit changes */
        ecs_commit(world, entity);
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
    ECS_IMPORT(world, EcsComponentsInput, flags);

    ECS_COMPONENT(world, Sdl2Window);

    ECS_SYSTEM(world, SdlInitWindow, EcsOnSet, EcsCanvas2D, HANDLE.Sdl2Window, HANDLE.EcsInput);
    ECS_SYSTEM(world, SdlDeinitWindow, EcsOnRemove, Sdl2Window);
    ECS_SYSTEM(world, SdlDeinit, EcsOnRemove, 0);

    ECS_SYSTEM(world, SdlDraw2DShapesChildren, EcsOnDemand,
        EcsPosition2D, ?EcsRotation2D, ?EcsScale2D, ?EcsColor,
        EcsSquare | EcsRectangle | EcsCircle | EcsTriangle,
        HANDLE.EcsSquare, HANDLE.EcsRectangle, HANDLE.EcsCircle, HANDLE.EcsTriangle,
        ?EcsContainer);

    ECS_SYSTEM(world, SdlDraw2DShapes, EcsOnDemand,
        EcsPosition2D, ?EcsRotation2D, ?EcsScale2D, ?EcsColor,
        EcsSquare | EcsRectangle | EcsCircle | EcsTriangle,
        HANDLE.EcsSquare, HANDLE.EcsRectangle, HANDLE.EcsCircle, HANDLE.EcsTriangle,
        ?EcsContainer, COMPONENT.Sdl2Window, COMPONENT.EcsCanvas2D, HANDLE.SdlDraw2DShapesChildren);

    ECS_SYSTEM(world, Sdl2Input, EcsPreFrame, EcsInput);
    ECS_SYSTEM(world, Sdl2Render, EcsPostFrame, Sdl2Window, HANDLE.SdlDraw2DShapes);

    ECS_FAMILY(world, Sdl2, SdlInitWindow, SdlDeinitWindow, Sdl2Input, Sdl2Render);

    ecs_add(world, SdlInitWindow_h, EcsHidden_h);
    ecs_add(world, SdlDeinitWindow_h, EcsHidden_h);
    ecs_add(world, Sdl2Input_h, EcsHidden_h);
    ecs_add(world, Sdl2Render_h, EcsHidden_h);
    ecs_commit(world, SdlInitWindow_h);
    ecs_commit(world, SdlDeinitWindow_h);
    ecs_commit(world, Sdl2Input_h);
    ecs_commit(world, Sdl2Render_h);

    SDL_Init(SDL_INIT_VIDEO | SDL_WINDOW_OPENGL);

    handles->Sdl2 = Sdl2_h;
    handles->Sdl2Input = Sdl2Input_h;
    handles->Sdl2Render = Sdl2Render_h;
}
