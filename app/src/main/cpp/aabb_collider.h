//
// Created by jsmar on 07/06/2021.
//

#ifndef QUEST_DEMO_AABB_COLLIDER_H
#define QUEST_DEMO_AABB_COLLIDER_H

#include <VrApi.h>

#include "math.h"
#include "mesh_renderer.h"

#define MAX_SCENE_COLLIDERS 100

/**
 * AABB collider utility.
 * Collision resolution will come later
 * A collider can be entangles to a scene object, and move and rotate with it
 * */

struct sAABB {
    sVector3  origin_point   = sVector3{ 0.0f, 0.0f, 0.0f};
    float     width          = 0.0f;
    float     height         = 0.0f;
    float     depth          = 0.0f;
};

#define DEBUG TRUE

struct sAABBColliderController {
    bool      enabled_colliders             [MAX_SCENE_COLLIDERS] = { false };
    int       collider_object_entanglement  [MAX_SCENE_COLLIDERS] = { -1 };

    sAABB     collider_position             [MAX_SCENE_COLLIDERS];
    sVector3  entangled_position_delta      [MAX_SCENE_COLLIDERS] = {sVector3{0.0f, 0.f, 0.0f}};

    // TODO: Add labels for the collisions
    // TODO: Add triggers for collisions

    sMeshRenderer debug_renderer;
};

void AABB_CONTROLLER_init(sAABBColliderController *col_contr);

int AABB_CONTROLLER_add_collider(sAABBColliderController *controller,
                                 const sVector3 position,
                                 const float width,
                                 const float heigth,
                                 const float depth);

void AABB_COLLIDER_update(sAABBColliderController  *col_contr,
                          const sVector3           *obj_positions,
                          const sQuaternion4       *obj_rotations);

void AABB_CONTROLLER_render(const sAABBColliderController *col_contr,
                            const ovrTracking2         *tracking,
                            const unsigned int         eye_index);


#endif //QUEST_DEMO_AABB_COLLIDER_H
