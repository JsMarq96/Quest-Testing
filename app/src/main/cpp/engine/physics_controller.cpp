//
// Created by jsmar on 22/06/2021.
//

#include "physics_controller.h"


void PHYS_init(sPhysicsController  *phys_controller,
               bool                *enabled_objs,
               sVector3            *obj_positions,
               sQuaternion4        *obj_rotation) {
    phys_controller->position = obj_positions;
    phys_controller->rotation = obj_rotation;
    phys_controller->enabled = enabled_objs;
}

void PHYS_resolve_collisions(sPhysicsController       *phys_controller,
                             const sCollisionManifold *manifolds,
                             const int                 manifold_count) {

    for(int i = 0; i < manifold_count; i++) {
        int index_a = manifolds[i].obj1_index;
        int index_b = manifolds[i].obj2_index;
        sVector3 col_normal = manifolds[i].collision_normal;

        sVector3 relative_speed = { phys_controller->pos_speed[index_a].x - phys_controller->pos_speed[index_b].x,
                                    phys_controller->pos_speed[index_a].y - phys_controller->pos_speed[index_b].y,
                                    phys_controller->pos_speed[index_a].z - phys_controller->pos_speed[index_b].z };
        float contact_speed = dot_prod(relative_speed, col_normal);

        //
        if (contact_speed > 0) {
            continue;
        }

        float impulse_scalar = (-1.0f * contact_speed) / (phys_controller->inv_mass[index_a] + phys_controller->inv_mass[index_b]);
        sVector3 impulse = { impulse_scalar * col_normal.x,
                             impulse_scalar * col_normal.y,
                             impulse_scalar * col_normal.z };

        PHYS_add_impulse(phys_controller,
                         index_b,
                         impulse);
        PHYS_add_impulse(phys_controller,
                         index_a,
                         sVector3{-impulse.x,
                                  -impulse.y,
                                  -impulse.z});
    }
}

void PHYS_update(sPhysicsController  *phys_controller,
                 const double        delta_time) {
    for(int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        if (!phys_controller->enabled[i] || (phys_controller->attributes[i] & IS_UNMOVABLE)) {
            continue;
        }
        // Add gravity
        if (phys_controller->attributes[i] & AFFECTED_GRAVITY) {

        }
        //phys_controller->pos_speed[i].y -= (GRAVITY_CONSTANT / phys_controller->mass[i]);

        sVector3 temp = phys_controller->position[i];

        /*phys_controller->position[i].x += phys_controller->pos_speed[i].x * delta_time;
        phys_controller->position[i].y += phys_controller->pos_speed[i].y * delta_time;
        phys_controller->position[i].z += phys_controller->pos_speed[i].z * delta_time;*/

    }
}