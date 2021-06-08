//
// Created by jsmar on 07/06/2021.
//

#include "aabb_collider.h"


void AABB_CONTROLLER_init(sAABBColliderController *col_contr) {

    render_init_cube(&col_contr->debug_renderer, true);
}

int AABB_CONTROLLER_add_collider(sAABBColliderController *controller,
                                  const sVector3 position,
                                  const float width,
                                  const float heigth,
                                  const float depth) {
    int index = -1;
    for (int i = 0; i < MAX_SCENE_COLLIDERS; i++) {
        if (!controller->enabled_colliders[i]) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return index; // Early stop is there is no more space for colliders
    }

    controller->enabled_colliders[index] = true;
    controller->collider_position[index] = sAABB{position, width, heigth, depth};

    return index;
}

void AABB_COLLIDER_update(sAABBColliderController  *col_contr,
                          const sVector3           *obj_positions,
                          const sQuaternion4       *obj_rotations) {

    for(int i = 0; i <MAX_SCENE_COLLIDERS; i++) {
        if (!col_contr->enabled_colliders[i] || col_contr->collider_object_entanglement[i] == -1) {
            continue;
        }
        int obj_index = col_contr->collider_object_entanglement[i];

        sVector4 new_pos = sVector4 { col_contr->entangled_position_delta[i].x ,
                                     col_contr->entangled_position_delta[i].y,
                                     col_contr->entangled_position_delta[i].z,
                                      1.0f };
        sMat44 rot_matrix;
        convert_quaternion_to_matrix(&obj_rotations[obj_index], &rot_matrix);
        new_pos = rot_matrix.multiply(new_pos);

        col_contr->collider_position[i].origin_point = sVector3{ obj_positions[obj_index].x + new_pos.x,
                                                                 obj_positions[obj_index].y + new_pos.y,
                                                                 obj_positions[obj_index].z + new_pos.z };
    }
}

// NOTE: This is only for debug reasons
void AABB_CONTROLLER_render(const sAABBColliderController *col_contr,
                            const ovrTracking2            *tracking,
                            const unsigned int            eye_index) {

    for(int i = 0; i < MAX_SCENE_COLLIDERS; i++) {
        if (col_contr->enabled_colliders[i]) {
            const sAABB *curr_collider = &col_contr->collider_position[i];
            sMat44 model;
            model.set_position(curr_collider->origin_point);
            model.set_scale(sVector3{curr_collider->width, curr_collider->height, curr_collider->depth});

            render_mesh(&col_contr->debug_renderer,
                        &model,
                        tracking,
                        eye_index,
                        true);
        }
    }
}