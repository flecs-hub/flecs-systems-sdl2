
#include "sdl_private.h"

#include "systems.h"

static
void SdlDeinit(ecs_rows_t *rows) {
    SDL_Quit();
}

void EcsSystemsSdl2(
    ecs_world_t *world,
    int flags,
    void *handles_out)
{
    EcsSystemsSdl2Handles *handles = handles_out;

    ECS_IMPORT(world, EcsComponentsGraphics, flags);
    ECS_IMPORT(world, EcsComponentsGeometry, flags);
    ECS_IMPORT(world, EcsComponentsTransform, flags);
    ECS_IMPORT(world, EcsComponentsInput, flags);
    ECS_IMPORT(world, EcsSystemsTransform, flags);

    ECS_COMPONENT(world, SdlWindow);

    /* Camera transformation */
    ECS_SYSTEM(world, SdlCameraTransform2D, EcsOnUpdate, EcsCamera2D, CONTAINER.EcsCanvas2D, ID.SdlWindow, SYSTEM.EcsHidden);


    /* -- SDL / Window initialization -- */

    ECS_SYSTEM(world, SdlInitWindow, EcsOnSet, EcsCanvas2D, ID.SdlWindow, ID.EcsInput, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, SdlDeinitWindow, EcsOnRemove, SdlWindow, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, SdlDeinit, EcsOnRemove, 0, SYSTEM.EcsHidden);


    /* -- Input processing -- */

    ECS_SYSTEM(world, SdlInput, EcsOnLoad, EcsInput, ID.EcsCanvas2D, SYSTEM.EcsHidden);


    /* -- Rendering -- */

    ECS_SYSTEM(world, SdlRenderSquare, EcsManual,
        EcsSquare, EcsMatTransform2D, ?EcsColor, ?EcsLineColor,
        SYSTEM.EcsHidden
    );

    ECS_SYSTEM(world, SdlRenderRectangle, EcsManual,
        EcsRectangle, EcsMatTransform2D, ?EcsColor, ?EcsLineColor,
        SYSTEM.EcsHidden
    );

    ECS_SYSTEM(world, SdlRenderCircle, EcsManual,
        EcsCircle, EcsMatTransform2D, ?EcsColor, ?EcsLineColor,
        SYSTEM.EcsHidden
    );

    ECS_SYSTEM(world, SdlRender2D, EcsOnStore,
        SdlWindow,
        ID.SdlRenderSquare,
        ID.SdlRenderRectangle,
        ID.SdlRenderCircle,
        SYSTEM.EcsHidden
    );

    /* Types */
    ECS_TYPE(world, Sdl, SdlInitWindow, SdlDeinitWindow, SdlInput, SdlRender2D);

    SDL_Init(SDL_INIT_VIDEO | SDL_WINDOW_OPENGL);

    ECS_SET_ENTITY(handles, Sdl);
    ECS_SET_ENTITY(handles, SdlInput);
    ECS_SET_ENTITY(handles, SdlRender2D);
}
