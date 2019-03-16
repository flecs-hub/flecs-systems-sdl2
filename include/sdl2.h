#ifndef FLECS_SYSTEMS_SDL2_H
#define FLECS_SYSTEMS_SDL2_H

#include "bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemsSdl2Handles {
    ECS_DECLARE_COMPONENT(Sdl);
    ECS_DECLARE_SYSTEM(SdlInput);
    ECS_DECLARE_SYSTEM(SdlRender2D);
} EcsSystemsSdl2Handles;

void EcsSystemsSdl2(
    EcsWorld *world,
    int flags,
    void *handles_out);

#define EcsSystemsSdl2_ImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Sdl);\
    ECS_IMPORT_SYSTEM(handles, SdlInput);\
    ECS_IMPORT_SYSTEM(handles, SdlRender2D);

#ifdef __cplusplus
}
#endif

#endif
