
#include <include/sdl2.h>
#include <SDL2/SDL.h>
#include "SDL2_gfxPrimitives.h"

typedef struct SdlWindow {
    SDL_Window *window;
    SDL_Renderer *display;
    EcsMat3x3 screen; /* viewport coords to screen coords */
    EcsMat3x3 projection; /* screen + camera transform */
    EcsVec2 scale; /* when only scaling is required */
} SdlWindow;
