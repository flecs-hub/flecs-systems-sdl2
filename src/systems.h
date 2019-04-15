
#ifndef FLECS_SYSTEMS_SYSTEMS_H
#define FLECS_SYSTEMS_SYSTEMS_H

#include <include/bake_config.h>

void SdlCameraTransform2D(ecs_rows_t *rows);

void SdlInput(ecs_rows_t *rows);

void SdlRenderRectangle(ecs_rows_t *rows);

void SdlRenderSquare(ecs_rows_t *rows);

void SdlRenderCircle(ecs_rows_t *rows);

void SdlRenderPolygon8(ecs_rows_t *rows);

void SdlRender2D(ecs_rows_t *rows);

void SdlInitWindow(ecs_rows_t *rows);

void SdlDeinitWindow(ecs_rows_t *rows);

#endif