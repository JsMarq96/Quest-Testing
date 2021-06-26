//
// Created by jsmar on 22/06/2021.
//

#ifndef QUEST_DEMO_PHYSICS_CONTROLLER_H
#define QUEST_DEMO_PHYSICS_CONTROLLER_H

#include "collision_types.h"

#include "../utils/math.h"

#define MAX_INSTANCE_SIZE 100
#define GRAVITY_CONSTANT 0.981f

enum ePhysicsAttributes : unsigned char {
    IS_UNMOVABLE       = 0b0001,
    AFFECTED_GRAVITY   = 0b0010
};

struct sPhysicsController {
    /// POINTER THE CURRENT SCENE'S DATA
    sVector3       *position;
    sQuaternion4   *rotation;
    bool           *enabled;

    /// PHYSICS DATA
    float          mass          [MAX_INSTANCE_SIZE] = {0.00f};
    float          inv_mass      [MAX_INSTANCE_SIZE] = {0.00f};
    sVector3       pos_speed     [MAX_INSTANCE_SIZE] = {sVector3{}};
    sVector3       rot_speed     [MAX_INSTANCE_SIZE] = {sVector3{}};
    sVector3       acceleration  [MAX_INSTANCE_SIZE] = {sVector3{}};
    unsigned char  attributes    [MAX_INSTANCE_SIZE] = { 0b00 };
};

void PHYS_init(sPhysicsController  *phys_controller,
               bool                *enabled_objs,
               sVector3            *obj_positions,
               sQuaternion4        *obj_rotation);

void PHYS_resolve_collisions(sPhysicsController       *phys_controller,
                             const sCollisionManifold *manifolds,
                             const int                 manifold_count);

void PHYS_update(sPhysicsController  *phys_controller,
                 const double        delta_time);

inline void PHYS_add_impulse(sPhysicsController *phys_controller,
                             const int           index,
                             const sVector3      impulse) {
    float inv_mass = phys_controller->inv_mass[index];
    sVector3 tmp = phys_controller->pos_speed[index];
    tmp.x += inv_mass * impulse.x;
    tmp.y += inv_mass * impulse.y;
    tmp.z += inv_mass * impulse.z;

    phys_controller->pos_speed[index] = tmp;
}

/// INLINE FUNCTIONS

inline void PHYS_set_mass(sPhysicsController  *phys,
                          const int           index,
                          const float         mass) {
    phys->mass[index] = mass;
    phys->inv_mass[index] = 1.0f / mass;
}


#endif //QUEST_DEMO_PHYSICS_CONTROLLER_H
