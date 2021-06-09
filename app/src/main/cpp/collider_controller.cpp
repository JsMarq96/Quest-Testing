//
// Created by jsmar on 07/06/2021.
//

#include "collider_controller.h"

//// LIFECYCLE FUNCTIONS

void CC_init(sColliderController *col_contr) {

    render_init_cube(&col_contr->debug_renderer, true);
}

void CC_update(sColliderController    *col_contr,
               const sVector3         *obj_positions,
               const sQuaternion4     *obj_rotations,
               sCollision             *result_collisions,
               int                    *collision_count) {
    int enabled_collider_count = 0;
    int enabled_collider_indexing[MAX_SCENE_COLLIDERS] = {-1};

    // Move colliders if needed
    for(int i = 0; i <MAX_SCENE_COLLIDERS; i++) {
        if (!col_contr->enabled_colliders[i]) {
            continue;
        }
        enabled_collider_indexing[enabled_collider_count++] = i;

        if (col_contr->collider_object_entanglement[i] == -1) {
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

        col_contr->collider_origin_points[i] = sVector3{ obj_positions[obj_index].x + new_pos.x,
                                                         obj_positions[obj_index].y + new_pos.y,
                                                         obj_positions[obj_index].z + new_pos.z };

        // TODO: Box collider rotation, this works for sphere and AABB
    }

    // Detect collisions
    int collision_index = 0;
    // TODO: Naive bubblesort, fine for this stage, but could need some previus optimization
    for(int i = 0; i < enabled_collider_count; i++) {
        for(int j = i; j < enabled_collider_count; j++) {
            int index_1 = enabled_collider_indexing[i];
            int index_2 = enabled_collider_indexing[j];

            bool collision_detected = false;
            if (col_contr->collider_type[index_1] == col_contr->collider_type[index_2]) {
                switch(index_1) {
                    case AABB_COLLIDER:
                        if (test_AABB_AABB_collision(col_contr->collider_origin_points[index_1],
                                                     col_contr->box_collider_sizes[index_1],
                                                     col_contr->collider_origin_points[index_2],
                                                     col_contr->box_collider_sizes[index_2])) {
                            collision_detected = true;
                        }
                        break;
                    case SPHERE_COLLIDER:
                        if (test_sphere_sphere_collision(col_contr->collider_origin_points[index_1],
                                                         col_contr->sphere_collider_radius[index_1],
                                                         col_contr->collider_origin_points[index_2],
                                                         col_contr->sphere_collider_radius[index_2])) {
                            collision_detected = true;
                        }
                        break;
                    case BOX_COLLIDER:
                        // TODO: add box collider
                        break;
                }
            }

            if (collision_detected) {
                result_collisions[collision_index].collider1_index = index_1;
                result_collisions[collision_index].collider2_index = index_2;

                result_collisions[collision_index].collider1_entangled_object = col_contr->collider_object_entanglement[index_1];
                result_collisions[collision_index].collider2_entangled_object = col_contr->collider_object_entanglement[index_2];

                memcpy( result_collisions[collision_index].collider1_tag, col_contr->collider_tag[index_1], sizeof(char) * TAG_SIZE);
                memcpy( result_collisions[collision_index].collider2_tag, col_contr->collider_tag[index_2], sizeof(char) * TAG_SIZE);

                collision_index++;
            }
        }
    }

    *collision_count = collision_index;
}

// NOTE: This is only for debug reasons
void CC_render(const sColliderController *col_contr,
               const ovrTracking2            *tracking,
               const unsigned int            eye_index) {

    /*for(int i = 0; i < MAX_SCENE_COLLIDERS; i++) {
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
    }*/
}

//// COLLIDER MANAGING FUNCTIONS

int CC_add_AABB_collider(sColliderController *controller,
                         const sVector3      position,
                         const float         width,
                         const float         heigth,
                         const float         depth) {
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
    controller->collider_type[index] = AABB_COLLIDER;
    controller->collider_origin_points[index] = position;
    controller->box_collider_sizes[index] = sVector3{width, heigth, depth};

    return index;
}

int CC_add_sphere_collider(sColliderController *controller,
                           const sVector3       position,
                           const float          radius) {
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
    controller->collider_type[index] = SPHERE_COLLIDER;
    controller->collider_origin_points[index] = position;
    controller->sphere_collider_radius[index] = radius;

    return index;
}

int CC_add_Box_collider(sColliderController *controller,
                        const sVector3       position,
                        const sQuaternion4   rotation,
                        const float          width,
                        const float          heigth,
                        const float          depth) {
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
    controller->collider_type[index] = BOX_COLLIDER;
    controller->collider_origin_points[index] = position;
    controller->box_collider_sizes[index] = sVector3{width, heigth, depth};
    controller->box_collider_rotations[index] = rotation;

    return index;
}