
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

    ECS_COMPONENT(world, SdlWindow);


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


    /* -- Transformations -- */

    /* Apply camera transformation */
    ECS_SYSTEM(world, SdlCameraTransform2D, EcsOnFrame, EcsCamera2D, CONTAINER.EcsRoot);
    ecs_add(world, SdlCameraTransform2D_h, EcsHidden_h);

    /* System that adds transform matrix to every entity with transformations */
    ECS_SYSTEM(world, SdlAddMatTransform2D, EcsOnLoad, EcsPosition2D | EcsRotation2D | EcsScale2D, !EcsMatTransform2D);
    ecs_add(world, SdlAddMatTransform2D_h, EcsHidden_h);

    /* Systems that add transformations to transform matrix */
    ECS_SYSTEM(world, SdlApplyTranslation2D, EcsOnDemand, EcsMatTransform2D, EcsPosition2D);
    ECS_SYSTEM(world, SdlApplyRotation2D, EcsOnDemand, EcsMatTransform2D, EcsRotation2D);
    ECS_SYSTEM(world, SdlApplyScaling2D, EcsOnDemand, EcsMatTransform2D, EcsScale2D);
    ecs_add(world, SdlApplyTranslation2D_h, EcsHidden_h);
    ecs_add(world, SdlApplyRotation2D_h, EcsHidden_h);
    ecs_add(world, SdlApplyScaling2D_h, EcsHidden_h);

    /* Copy transformation from parent to child entities */
    ECS_SYSTEM(world, SdlInitTransformChildren2D, EcsOnDemand, EcsMatTransform2D, CONTAINER.EcsMatTransform2D);
    ecs_add(world, SdlInitTransformChildren2D_h, EcsHidden_h);

    /* System that applies transforms on child entities (invoked recursively) */
    ECS_SYSTEM(world, SdlTransformChildren2D, EcsOnDemand,
        EcsContainer,
        ID.SdlInitTransformChildren2D,
        ID.SdlApplyTranslation2D,
        ID.SdlApplyRotation2D,
        ID.SdlApplyScaling2D,
        !EcsRoot
    );

    /* System that applies transforms on top-level entities */
    ECS_SYSTEM(world, SdlTransform2D, EcsPostFrame,
        EcsRoot,
        ID.SdlInitTransformChildren2D,
        ID.SdlApplyTranslation2D,
        ID.SdlApplyRotation2D,
        ID.SdlApplyScaling2D,
        ID.SdlTransformChildren2D
    );

    ecs_add(world, SdlTransformChildren2D_h, EcsHidden_h);
    ecs_add(world, SdlTransform2D_h, EcsHidden_h);


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
