#ifndef FLECS_SYSTEMS_SDL2_WINDOW_H
#define FLECS_SYSTEMS_SDL2_WINDOW_H

#include <flecs-systems-sdl2/bake_config.h>

#ifdef __cplusplus
extern "C" {
#endif

ECS_STRUCT(Sdl2Window, {
    SDL_Window *window;
    int actual_width;
    int actual_height;
});

FLECS_SYSTEMS_SDL2_API
void FlecsSystemsSdl2WindowImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif
