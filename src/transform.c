#include "sdl_private.h"

void SdlAddMatTransform2D(EcsRows *rows) {
    EcsWorld *world = rows->world;
    EcsEntity EcsMatTransform2D_h = ecs_component(rows, 1);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        ecs_add(world, entity, EcsMatTransform2D_h);
    }
}

void SdlInitTransformChildren2D(EcsRows *rows) {
    EcsMatTransform2D *m_container = ecs_data(rows, NULL, 1);

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsMatTransform2D *m = ecs_data(rows, row, 0);
        *m = *m_container;
    }
}

void SdlApplyTranslation2D(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsMatTransform2D *m = ecs_data(rows, row, 0);
        EcsPosition2D *t = ecs_data(rows, row, 1);
        ecs_mat3x3_add_translation(m, t);
    }
}

void SdlApplyRotation2D(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsMatTransform2D *m = ecs_data(rows, row, 0);
        EcsRotation2D *t = ecs_data(rows, row, 1);
        ecs_mat3x3_add_rotation(m, t->angle);
    }
}

void SdlApplyScaling2D(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsMatTransform2D *m = ecs_data(rows, row, 0);
        EcsScale2D *t = ecs_data(rows, row, 1);
        ecs_mat3x3_add_scale(m, t);
    }
}

void SdlTransformChildren2D(EcsRows *rows) {
    EcsWorld *world = rows->world;
    float dt = rows->delta_time;
    EcsEntity SdlInitTransformChildren2D_h = ecs_component(rows, 1);
    EcsEntity SdlApplyTranslation2D_h = ecs_component(rows, 2);
    EcsEntity SdlApplyRotation2D_h = ecs_component(rows, 3);
    EcsEntity SdlApplyScaling2D_h = ecs_component(rows, 4);
    EcsEntity SdlTransformChildren2D_h = rows->system;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        ecs_run_w_filter(world, SdlInitTransformChildren2D_h, dt, 0, 0, entity, NULL);
        ecs_run_w_filter(world, SdlApplyTranslation2D_h, dt, 0, 0, entity, NULL);
        ecs_run_w_filter(world, SdlApplyRotation2D_h, dt, 0, 0, entity, NULL);
        ecs_run_w_filter(world, SdlApplyScaling2D_h, dt, 0, 0, entity, NULL);
        ecs_run_w_filter(world, SdlTransformChildren2D_h, dt, 0, 0, entity, NULL);
    }
}

void SdlTransform2D(EcsRows *rows) {
    EcsWorld *world = rows->world;
    float dt = rows->delta_time;
    EcsEntity root = ecs_entity(rows, rows->first, 0);
    EcsEntity SdlInitTransformChildren2D_h = ecs_component(rows, 1);
    EcsEntity SdlApplyTranslation2D_h = ecs_component(rows, 2);
    EcsEntity SdlApplyRotation2D_h = ecs_component(rows, 3);
    EcsEntity SdlApplyScaling2D_h = ecs_component(rows, 4);
    EcsEntity SdlTransformChildren2D_h = ecs_component(rows, 5);

    ecs_run_w_filter(world, SdlInitTransformChildren2D_h, dt, 0, 0, root, NULL);
    ecs_run_w_filter(world, SdlApplyTranslation2D_h, dt, 0, 0, root, NULL);
    ecs_run_w_filter(world, SdlApplyRotation2D_h, dt, 0, 0, root, NULL);
    ecs_run_w_filter(world, SdlApplyScaling2D_h, dt, 0, 0, root, NULL);
    ecs_run_w_filter(world, SdlTransformChildren2D_h, dt, 0, 0, root, NULL);
}
