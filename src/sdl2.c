#include <include/sdl2.h>
#include <SDL2/SDL.h>

typedef struct Sdl2Window {
    SDL_Window *window;
    SDL_Renderer *display;
} Sdl2Window;

static
void PollWindow(
    EcsWorld *world,
    Sdl2Window *wnd)
{
    SDL_Event e;
    SDL_SetRenderDrawColor(wnd->display, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(wnd->display);
    SDL_RenderPresent(wnd->display);
    SDL_PollEvent(&e);

    if (e.type == SDL_QUIT) {
        ecs_quit(world);
    }
}

static
void SdlPoll(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Sdl2Window *wnd = ecs_column(rows, row, 0);
        PollWindow(rows->world, wnd);
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

    ECS_COMPONENT(world, Sdl2Window);

    ECS_SYSTEM(world, SdlInitWindow, EcsOnSet, EcsCanvas2D, HANDLE.Sdl2Window);
    ECS_SYSTEM(world, SdlDeinitWindow, EcsOnRemove, Sdl2Window);
    ECS_SYSTEM(world, SdlDeinit, EcsOnRemove, 0);
    ECS_SYSTEM(world, SdlPoll, EcsOnFrame, Sdl2Window);

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
