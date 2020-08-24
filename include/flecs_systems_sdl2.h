#ifndef FLECS_SYSTEMS_SDL2_H
#define FLECS_SYSTEMS_SDL2_H

#include <flecs-systems-sdl2/bake_config.h>
#include <flecs-systems-sdl2/window.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlecsSystemsSdl2 {
    ECS_DECLARE_ENTITY(Sdl);
} FlecsSystemsSdl2;

FLECS_SYSTEMS_SDL2_EXPORT
void FlecsSystemsSdl2Import(
    ecs_world_t *world);

#define FlecsSystemsSdl2ImportHandles(handles)\
    ECS_IMPORT_ENTITY(handles, Sdl);

#ifdef __cplusplus
}
#endif

#endif
