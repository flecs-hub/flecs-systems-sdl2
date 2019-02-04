#include "sdl_private.h"

void SdlInitWindow(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity SdlWindow_h = ecs_component(rows, 1);
    EcsEntity EcsInput_h = ecs_component(rows, 2);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsCanvas2D *canvas = ecs_data(rows, row, 0);
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);

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
            ecs_quit(world);
            break;
        }

        /* If viewport is not set explicitly, inherit from window */
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
            ecs_quit(world);
            break;
        }


        /* Create matrix that translates coordinates to canvas space */
        EcsMat3x3 screen = ECS_MAT3X3_IDENTITY;
        EcsVec2 scale = {
            (float)canvas->window.width / (float)canvas->viewport.width,
            (float)canvas->window.height / (float)canvas->viewport.height
        };
        EcsVec2 translate = {
            canvas->window.width / 2,
            canvas->window.height / 2
        };

        ecs_mat3x3_add_scale(&screen, &scale);
        ecs_mat3x3_add_translation(&screen, &translate);

        /* Add SdlWindow component that stores window and renderer */
        ecs_set(world, entity, SdlWindow, {
            .window = window,
            .display = display,
            .screen = screen,
            /* projection is equal to screen when there is no camera */
            .projection = screen,
            .scale = scale
        });

        /* Add EcsInput component */
        ecs_add(world, entity, EcsInput_h);
    }
}

void SdlDeinitWindow(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        SdlWindow *window = ecs_data(rows, row, 0);
        SDL_DestroyWindow(window->window);
    }
}
