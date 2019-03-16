
#ifndef FLECS_SYSTEMS_SYSTEMS_H
#define FLECS_SYSTEMS_SYSTEMS_H

#include <include/bake_config.h>

void SdlCameraTransform2D(EcsRows *rows);

void SdlInput(EcsRows *rows);

void SdlRenderRectangle(EcsRows *rows);

void SdlRenderSquare(EcsRows *rows);

void SdlRenderCircle(EcsRows *rows);

void SdlRender2D(EcsRows *rows);

void SdlInitWindow(EcsRows *rows);

void SdlDeinitWindow(EcsRows *rows);

#endif