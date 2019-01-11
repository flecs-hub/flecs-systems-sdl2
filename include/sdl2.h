#ifndef REFLECS_SYSTEMS_SDL2_H
#define REFLECS_SYSTEMS_SDL2_H

#include "bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemsSdl2Handles {
   EcsHandle Sdl2;
   EcsHandle Sdl2Input;
   EcsHandle Sdl2Render;
} EcsSystemsSdl2Handles;

void EcsSystemsSdl2(
    EcsWorld *world,
    int flags,
    void *handles_out);

#define EcsSystemsSdl2_DeclareHandles(handles)\
    EcsDeclareHandle(handles, Sdl2);\
    EcsDeclareHandle(handles, Sdl2Input);\
    EcsDeclareHandle(handles, Sdl2Render);

#ifdef __cplusplus
}
#endif

#endif
