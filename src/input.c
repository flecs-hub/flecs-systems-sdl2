#include "sdl_private.h"

static
uint32_t key_sym(
    uint32_t sdl_sym)
{
    if (sdl_sym < 128) return sdl_sym;
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
    EcsKeyState *key)
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
    EcsKeyState *key)
{
    key->current = false;
}

void SdlInput(EcsRows *rows) {
    EcsWorld *world = rows->world;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsInput *input = ecs_data(rows, row, 0);

        /* Reset key state array */
        int i;
        for (i = 0; i < 128; i ++) {
            EcsKeyState *state = &input->keys[i];
            if (!state->current) {
                state->state = 0;
                state->pressed = 0;
            } else if (state->state) {
                state->pressed = 0;
            }
        }

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                ecs_quit(world);

            } else if (e.type == SDL_KEYDOWN) {
                uint32_t sym = key_sym(e.key.keysym.sym);
                key_down(&input->keys[sym]);

            } else if (e.type == SDL_KEYUP) {
                uint32_t sym = key_sym(e.key.keysym.sym);
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
