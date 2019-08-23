
#include "sdl_private.h"
#include "systems.h"

static
void SdlDeinit(ecs_rows_t *rows) {
    SDL_Quit();
}

void FlecsSystemsSdl2Import(
    ecs_world_t *world,
    int flags)
{
    ECS_IMPORT(world, FlecsComponentsGraphics, flags);
    ECS_IMPORT(world, FlecsComponentsGeometry, flags);
    ECS_IMPORT(world, FlecsComponentsTransform, flags);
    ECS_IMPORT(world, FlecsComponentsInput, flags);
    ECS_IMPORT(world, FlecsSystemsTransform, flags);

    ECS_MODULE(world, FlecsSystemsSdl2);

    ECS_COMPONENT(world, SdlWindow);

    /* Camera transformation */
    ECS_SYSTEM(world, SdlCameraTransform2D, EcsOnUpdate, EcsCamera2D, CONTAINER.EcsCanvas2D, .SdlWindow, SYSTEM.EcsHidden);


    /* -- SDL / Window initialization -- */

    ECS_SYSTEM(world, SdlInitWindow, EcsOnSet, EcsCanvas2D, .SdlWindow, .EcsInput, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, SdlDeinitWindow, EcsOnRemove, SdlWindow, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, SdlDeinit, EcsOnRemove, SYSTEM.EcsHidden);


    /* -- Input processing -- */

    ECS_SYSTEM(world, SdlInput, EcsOnLoad, EcsInput, .EcsCanvas2D, SYSTEM.EcsHidden);


    /* -- Rendering -- */

    ECS_SYSTEM(world, SdlRenderLine, EcsManual,
        EcsLine, EcsMatTransform2D, ?EcsLineColor,
        SYSTEM.EcsHidden
    );

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

    ECS_SYSTEM(world, SdlRenderPolygon8, EcsManual,
        EcsPolygon8, EcsMatTransform2D, ?EcsColor, ?EcsLineColor,
        SYSTEM.EcsHidden
    );

    ECS_SYSTEM(world, SdlRenderDot, EcsManual,
        EcsDot, EcsMatTransform2D, ?EcsColor,
        SYSTEM.EcsHidden
    );

    ECS_SYSTEM(world, SdlRender2D, EcsOnStore,
        SdlWindow,
        .SdlRenderSquare,
        .SdlRenderRectangle,
        .SdlRenderCircle,
        .SdlRenderPolygon8,
        .SdlRenderDot,
        .SdlRenderLine,
        SYSTEM.EcsHidden
    );

    /* Types */
    ECS_TYPE(world, Sdl, SdlInitWindow, SdlDeinitWindow, SdlInput, SdlRender2D);

    SDL_Init(SDL_INIT_VIDEO | SDL_WINDOW_OPENGL);

    ECS_SET_ENTITY(Sdl);
    ECS_SET_ENTITY(SdlInput);
    ECS_SET_ENTITY(SdlRender2D);
}
