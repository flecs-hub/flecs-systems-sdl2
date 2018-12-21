#include <include/sdl2.h>
#include <SDL2/SDL.h>

typedef struct Sdl2Window {
    SDL_Window *window;
    SDL_Renderer *display;
} Sdl2Window;

void SdlInitWindow(EcsRows *rows) {
    EcsWorld *world = rows->world;

    printf("InitWindow\n");

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsCanvas2D *canvas = ecs_column(rows, row, 0);
        EcsHandle Sdl2Window_h = ecs_handle(rows, 1);
        EcsHandle entity = ecs_entity(row);

        SDL_Window *window = SDL_CreateWindow(
            canvas->title,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            canvas->window.width,
            canvas->window.height,
            SDL_WINDOW_OPENGL
        );

        if (!window) {
            printf("SDL2 window creation failed for canvas\n");
            continue;
        }

        SDL_Renderer *display = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

        if (!display) {
            printf("SDL2 renderer creation failed for canvas: %s\n",
              SDL_GetError());
            continue;
        }

        ecs_set(world, entity, Sdl2Window, {
          .window = window,
          .display = display
        });
    }
}

void SdlDeinitWindow(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Sdl2Window *window = ecs_column(rows, row, 0);
        SDL_DestroyWindow(window->window);
    }
}

void SdlDeinit(EcsRows *rows) {
    SDL_Quit();
}

void EcsSystemsSdl2(
    EcsWorld *world,
    int flags,
    void *handles_out)
{
    bool do_2d = !flags || flags & ECS_2D;
    bool do_3d = !flags || flags & ECS_3D;
    EcsSystemsSdl2Handles *handles = handles_out;

    ECS_IMPORT(world, EcsComponentsGraphics, flags);

    ECS_COMPONENT(world, Sdl2Window);

    ECS_SYSTEM(world, SdlInitWindow, EcsOnAdd, EcsCanvas2D, HANDLE.Sdl2Window);
    ECS_SYSTEM(world, SdlDeinitWindow, EcsOnRemove, Sdl2Window);
    ECS_SYSTEM(world, SdlDeinit, EcsOnRemove, 0);

    ECS_FAMILY(world, Sdl2, SdlInitWindow, SdlDeinitWindow);

    ecs_add(world, SdlInitWindow_h, EcsHidden_h);
    ecs_add(world, SdlDeinitWindow_h, EcsHidden_h);
    ecs_commit(world, SdlInitWindow_h);
    ecs_commit(world, SdlDeinitWindow_h);

    //SDL_Init(SDL_INIT_VIDEO);

    handles->Sdl2 = Sdl2_h;

/*

    int active = 0;
    EcsVector2D position = {0,0};
    SDL_Event e;
    do {
        SDL_SetRenderDrawColor(display, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(display);
        EcsCollisionInfo *end = collisions;
        EcsColliderData **iter = colliders;
        while(*iter != NULL) {
            Color *color = &cWhite;
            if (*iter == colliders[active]) {
                color = &cGreen;
            } else {
                if (EcsPhysis2dCollisionCheck(*iter, colliders[active], end)) {
                    position[0] = end->direction[0];
                    position[1] = end->direction[1];
                    EcsVector2D_scale(&position, end->distance, &position);
                    MoveCollider(*iter, &position);
                    end++;
                }
            }
            DrawCollider(display, *iter, color);
            iter++;
        }
        EcsCollisionInfo *infoIter = collisions;
        while (infoIter < end) {
            DrawCollisionInfo(display, EcsColliderData_Vector2(colliders[active]), infoIter, &cRed);
            infoIter++;
        }
        SDL_RenderPresent(display);

        SDL_PollEvent(&e);
        if (e.type == SDL_KEYDOWN) {
            position[0] = 0;
            position[1] = 0;

            if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
                position[1] -= .1;
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                position[1] += .1;
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                position[0] -= .1;
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                position[0] += .1;
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_N) {
                active = (active+1)%COLLIDERS_COUNT;
            }
        }
    } while(e.type != SDL_QUIT);
*/
}
