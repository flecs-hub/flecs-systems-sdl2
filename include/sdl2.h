#ifndef REFLECS_SYSTEMS_SDL2_H
#define REFLECS_SYSTEMS_SDL2_H

#include "bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemsSdl2Handles {
   EcsHandle Sdl2;
} EcsSystemsSdl2Handles;

void EcsSystemsSdl2(
    EcsWorld *world,
    int flags,
    void *handles_out);

#define EcsSystemsSdl2_DeclareHandles(handles)\
    EcsDeclareHandle(handles, Sdl2);

#ifdef __cplusplus
}
#endif

#endif
