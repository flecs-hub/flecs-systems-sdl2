#ifndef FLECS_SYSTEMS_SDL2_H
#define FLECS_SYSTEMS_SDL2_H

#include <flecs-systems-sdl2/bake_config.h>

// Don't use reflection, but use utility macro's for auto-exporting variables
#undef ECS_META_IMPL
#ifndef flecs_systems_sdl2_EXPORTS
#define ECS_META_IMPL EXTERN
#else
#define ECS_META_IMPL DECLARE
#endif

#include <flecs-systems-sdl2/window.h>

#ifdef __cplusplus
extern "C" {
#endif

FLECS_SYSTEMS_SDL2_API
void FlecsSystemsSdl2Import(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#ifdef FLECS_CPP
#ifdef __cplusplus

namespace flecs {
namespace systems {

class sdl2 : FlecsSystemsSdl2 {
public:
    sdl2(flecs::world& ecs) {
        FlecsSystemsSdl2Import(ecs);
        ecs.module<flecs::systems::sdl2>();
    }
};

}
}

#endif
#endif

#endif
