#ifndef FLECS_SYSTEMS_SDL2_H
#define FLECS_SYSTEMS_SDL2_H

#include <flecs-systems-sdl2/bake_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlecsSystemsSdl2 {
    ECS_DECLARE_ENTITY(Sdl);
    ECS_DECLARE_ENTITY(SdlInput);
    ECS_DECLARE_ENTITY(SdlRender2D);
} FlecsSystemsSdl2;

void FlecsSystemsSdl2Import(
    ecs_world_t *world,
    int flags);

#define FlecsSystemsSdl2ImportHandles(handles)\
    ECS_IMPORT_ENTITY(handles, Sdl);\
    ECS_IMPORT_ENTITY(handles, SdlInput);\
    ECS_IMPORT_ENTITY(handles, SdlRender2D);

#ifdef __cplusplus
}
#endif

#endif
