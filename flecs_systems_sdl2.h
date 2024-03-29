// Comment out this line when using as DLL
#define flecs_systems_sdl2_STATIC
#ifndef FLECS_SYSTEMS_SDL2_H
#define FLECS_SYSTEMS_SDL2_H

/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef FLECS_SYSTEMS_SDL2_BAKE_CONFIG_H
#define FLECS_SYSTEMS_SDL2_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <flecs.h>
#include <flecs_components_gui.h>
#include <flecs_components_input.h>
#include <bake_sdl2.h>

/* Convenience macro for exporting symbols */
#ifndef flecs_systems_sdl2_STATIC
#if flecs_systems_sdl2_EXPORTS && (defined(_MSC_VER) || defined(__MINGW32__))
  #define FLECS_SYSTEMS_SDL2_API __declspec(dllexport)
#elif flecs_systems_sdl2_EXPORTS
  #define FLECS_SYSTEMS_SDL2_API __attribute__((__visibility__("default")))
#elif defined _MSC_VER
  #define FLECS_SYSTEMS_SDL2_API __declspec(dllimport)
#else
  #define FLECS_SYSTEMS_SDL2_API
#endif
#else
  #define FLECS_SYSTEMS_SDL2_API
#endif

#endif


// Don't use reflection, but use utility macro's for auto-exporting variables
#undef ECS_META_IMPL
#ifndef flecs_systems_sdl2_EXPORTS
#define ECS_META_IMPL EXTERN
#else
#define ECS_META_IMPL DECLARE
#endif

#ifndef FLECS_SYSTEMS_SDL2_WINDOW_H
#define FLECS_SYSTEMS_SDL2_WINDOW_H


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
