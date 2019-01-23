
#include "sdl_private.h"

static
void SdlDeinit(EcsRows *rows) {
    SDL_Quit();
}

void EcsSystemsSdl2(
    EcsWorld *world,
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
    ECS_SYSTEM(world, SdlCameraTransform2D, EcsOnFrame, EcsCamera2D, CONTAINER.EcsRoot);
    ecs_add(world, SdlCameraTransform2D_h, EcsHidden_h);


    /* -- SDL / Window initialization -- */

    ECS_SYSTEM(world, SdlInitWindow, EcsOnSet, EcsCanvas2D, ID.SdlWindow, ID.EcsInput, ID.EcsMatTransform2D);
    ECS_SYSTEM(world, SdlDeinitWindow, EcsOnRemove, SdlWindow);
    ECS_SYSTEM(world, SdlDeinit, EcsOnRemove, 0);
    ecs_add(world, SdlInitWindow_h, EcsHidden_h);
    ecs_add(world, SdlDeinitWindow_h, EcsHidden_h);
    ecs_add(world, SdlDeinit_h, EcsHidden_h);



    /* -- Input processing -- */

    ECS_SYSTEM(world, SdlInput, EcsOnLoad, EcsInput);
    ecs_add(world, SdlInput_h, EcsHidden_h);


    /* -- Rendering -- */

    ECS_SYSTEM(world, SdlRenderSquare, EcsOnDemand,
        CONTAINER.SdlWindow, EcsSquare, EcsMatTransform2D, ?EcsColor
    );

    ECS_SYSTEM(world, SdlRenderRectangle, EcsOnDemand,
        CONTAINER.SdlWindow, EcsRectangle, EcsMatTransform2D, ?EcsColor
    );

    ECS_SYSTEM(world, SdlRenderCircle, EcsOnDemand,
        CONTAINER.SdlWindow, EcsCircle, EcsMatTransform2D, ?EcsColor
    );

    ECS_SYSTEM(world, SdlRender2D, EcsOnStore,
        SdlWindow,
        ID.SdlRenderSquare,
        ID.SdlRenderRectangle,
        ID.SdlRenderCircle);

    ecs_add(world, SdlRenderSquare_h, EcsHidden_h);
    ecs_add(world, SdlRenderRectangle_h, EcsHidden_h);
    ecs_add(world, SdlRenderCircle_h, EcsHidden_h);
    ecs_add(world, SdlRender2D_h, EcsHidden_h);


    /* Families */
    ECS_FAMILY(world, Sdl, SdlInitWindow, SdlDeinitWindow, SdlInput, SdlRender2D);

    /* Mark systems as hidden */
    ecs_add(world, SdlRender2D_h, EcsHidden_h);

    SDL_Init(SDL_INIT_VIDEO | SDL_WINDOW_OPENGL);

    handles->Sdl = Sdl_h;
    handles->SdlInput = SdlInput_h;
    handles->SdlRender = SdlRender2D_h;
}
