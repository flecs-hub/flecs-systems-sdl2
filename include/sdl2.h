#ifndef FLECS_SYSTEMS_SDL2_H
#define FLECS_SYSTEMS_SDL2_H

#include "bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemsSdl2Handles {
    ECS_DECLARE_ENTITY(Sdl);
    ECS_DECLARE_ENTITY(SdlInput);
    ECS_DECLARE_ENTITY(SdlRender2D);
} EcsSystemsSdl2Handles;

void EcsSystemsSdl2(
    ecs_world_t *world,
    int flags,
    void *handles_out);

#define EcsSystemsSdl2_ImportHandles(handles)\
    ECS_IMPORT_ENTITY(handles, Sdl);\
    ECS_IMPORT_ENTITY(handles, SdlInput);\
    ECS_IMPORT_ENTITY(handles, SdlRender2D);

#ifdef __cplusplus
}
#endif

#endif
