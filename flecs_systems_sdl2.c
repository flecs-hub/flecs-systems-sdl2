#ifndef FLECS_SYSTEMS_SDL2_IMPL
#include "flecs_systems_sdl2.h"
#endif

ECS_COMPONENT_DECLARE(Sdl2Window);

static
uint32_t key_sym(
    uint32_t sdl_sym,
    bool shift)
{
    if (sdl_sym < 128) {
        if (shift) {
            if (sdl_sym == ECS_KEY_EQUAL) {
                sdl_sym = ECS_KEY_PLUS;
            } else if (sdl_sym == ECS_KEY_UNDERSCORE) {
                sdl_sym = ECS_KEY_MINUS;
            } else {
                return sdl_sym;
            }
        }
        return sdl_sym;
    }

    switch(sdl_sym) {
    case SDLK_RIGHT: return 'R';
    case SDLK_LEFT: return 'L';
    case SDLK_DOWN: return 'D';
    case SDLK_UP: return 'U';
    case SDLK_LCTRL: return 'C';
    case SDLK_LSHIFT: return 'S';
    case SDLK_LALT: return 'A';
    case SDLK_RCTRL: return 'C';
    case SDLK_RSHIFT: return 'S';
    case SDLK_RALT: return 'A';
    }
    return 0;
}

static
void key_down(
    ecs_key_state_t *key)
{
    if (key->state) {
        key->pressed = false;
    } else {
        key->pressed = true;
    }

    key->state = true;
    key->current = true;
}

static
void key_up(
    ecs_key_state_t *key)
{
    key->current = false;
}

static
void key_reset(
    ecs_key_state_t *state)
{
    if (!state->current) {
        state->state = 0;
        state->pressed = 0;
    } else if (state->state) {
        state->pressed = 0;
    }
}

static
void Sdl2ProcessEvents(ecs_iter_t *it) {
    EcsInput *input = ecs_field(it, EcsInput, 1);

    /* Reset key state array */
    int k;
    for (k = 0; k < 128; k ++) {
        key_reset(&input->keys[k]);
    }

    key_reset(&input->mouse.left);
    key_reset(&input->mouse.right);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            ecs_quit(it->world);

        } else if (e.type == SDL_KEYDOWN) {
            uint32_t sym = key_sym(e.key.keysym.sym, input->keys['S'].state != 0);
            key_down(&input->keys[sym]);

        } else if (e.type == SDL_KEYUP) {
            uint32_t sym = key_sym(e.key.keysym.sym, input->keys['S'].state != 0);
            key_up(&input->keys[sym]);

        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                key_down(&input->mouse.left);
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                key_down(&input->mouse.right);
            }

        } else if (e.type == SDL_MOUSEBUTTONUP) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                key_up(&input->mouse.left);
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                key_up(&input->mouse.right);
            }

        } else if (e.type == SDL_MOUSEMOTION) {
            input->mouse.wnd.x = e.motion.x;
            input->mouse.wnd.y = e.motion.y;
            input->mouse.rel.x = e.motion.xrel;
            input->mouse.rel.y = e.motion.yrel;

        } else if (e.type == SDL_MOUSEWHEEL) {
            input->mouse.scroll.x = e.wheel.x;
            input->mouse.scroll.y = e.wheel.y;
        }
    }
}

void sdl2_fini(ecs_world_t *world,void *ctx) {
    (void)ctx;
    SDL_Quit();
}

void FlecsSystemsSdl2Import(ecs_world_t *world) {
    ECS_MODULE(world, FlecsSystemsSdl2);
    ECS_IMPORT(world, FlecsComponentsInput);
    ECS_IMPORT(world, FlecsComponentsGui);
    ECS_IMPORT(world, FlecsSystemsSdl2Window); 

    /* Input system */
    ECS_SYSTEM(world, Sdl2ProcessEvents, EcsPostLoad, 
        flecs.components.input.Input($));
    ecs_set_ptr(world, ecs_id(EcsInput), EcsInput, NULL);

    /* SDL deinitialization */
    ecs_atfini(world, sdl2_fini, NULL);

    /* SDL initialization */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        ecs_err("Unable to initialize SDL: %s", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);    

    ecs_trace("SDL initialized");
}

static
void Sdl2CreateWindow(ecs_iter_t *it) {
    EcsCanvas *window = ecs_field(it, EcsCanvas, 1);
    ecs_entity_t ecs_id(Sdl2Window) = ecs_field_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        Sdl2Window *sdl_window = ecs_get_mut(it->world, e, Sdl2Window);

        const char *title = window[i].title;
        if (!title) {
            title = "SDL2 window";
        }

        int x = SDL_WINDOWPOS_UNDEFINED;
        int y = SDL_WINDOWPOS_UNDEFINED;

        /* Create SDL Window with support for OpenGL and high resolutions */
        SDL_Window *wnd = SDL_CreateWindow(
            title, x, y, window[i].width, window[i].height, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
        );

        if (!wnd) {
            ecs_err("SDL2 window creation failed: %s\n", SDL_GetError());
            break;
        }

        /* Set actual dimensions, as this may be a high resolution display */
        int32_t actual_width, actual_height;
        SDL_GL_GetDrawableSize(wnd, &actual_width, &actual_height);

        sdl_window->window = wnd;
        sdl_window->actual_width = actual_width;
        sdl_window->actual_height = actual_height;
    }
}

static
void Sdl2DestroyWindow(ecs_iter_t *it) {
    Sdl2Window *window = ecs_field(it, Sdl2Window, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        SDL_DestroyWindow(window[i].window);
    }
}

void FlecsSystemsSdl2WindowImport(ecs_world_t *world) {
    ECS_MODULE(world, FlecsSystemsSdl2Window);

    ecs_set_name_prefix(world, "Sdl2");

    ECS_COMPONENT_DEFINE(world, Sdl2Window);

    ECS_OBSERVER(world, Sdl2CreateWindow, EcsOnSet, 
        [in] flecs.components.gui.Canvas,
        [out] flecs.systems.sdl2.window.Window());

    ECS_OBSERVER(world, Sdl2DestroyWindow, EcsUnSet, 
        [in] flecs.systems.sdl2.window.Window());
}
