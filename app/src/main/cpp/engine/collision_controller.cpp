//
// Created by jsmar on 07/06/2021.
//
#include "collision_controller.h"


//// LIFECYCLE FUNCTIONS

void CC_init(sColliderController *col_contr) {
    memset(col_contr->collider_object_entanglement, -1, MAX_SCENE_COLLIDERS);
    render_init_cube(&col_contr->debug_renderer, true);
}

void CC_update(sColliderController    *col_contr,
               const sVector3         *obj_positions,
               const sQuaternion4     *obj_rotations,
               sCollisionManifold             *result_collisions,
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

        sVector3 tmp = sVector3{obj_positions[obj_index].x + new_pos.x,
                                               obj_positions[obj_index].y + new_pos.y,
                                               obj_positions[obj_index].z + new_pos.z };

        col_contr->origin_points[i] = tmp;

        col_contr->box_collider_rotations[i] = obj_rotations[obj_index];

        get_OBB_raw_vertex(col_contr->origin_points[i],
                           col_contr->box_collider_sizes[i],
                           col_contr->box_collider_rotations[i],
                           col_contr->collider_raw_vertex[i]);

        if (obj_rotations[obj_index].x  != col_contr->box_collider_rotations[i].x &&
                obj_rotations[obj_index].y != col_contr->box_collider_rotations[i].y &&
                obj_rotations[obj_index].z != col_contr->box_collider_rotations[i].z &&
                obj_rotations[obj_index].w != col_contr->box_collider_rotations[i].w &&
                obj_positions[obj_index].x != tmp.x &&
                obj_positions[obj_index].y != tmp.y &&
                obj_positions[obj_index].z != tmp.z) {

        }
    }

    // Detect collisions
    int collision_index = 0;
    // TODO: Naive bubblesort, fine for this stage, but could need some optimization on the future
    for(int i = 0; i < enabled_collider_count; i++) {
        for(int j = i+1; j < enabled_collider_count; j++) {
            int index_1 = enabled_collider_indexing[i];
            int index_2 = enabled_collider_indexing[j];

            sCollisionManifold manifold;

            if (SAT_OBB_v_OBB(col_contr->origin_points[index_1],
                              col_contr->box_collider_sizes[index_1],
                              col_contr->box_collider_rotations[index_1],
                              col_contr->origin_points[index_2],
                              col_contr->box_collider_sizes[index_2],
                              col_contr->box_collider_rotations[index_2],
                              &result_collisions[collision_index])) {

                info("COLLISION BEIBIIII %d %d %d", index_1, index_2, result_collisions[collision_index].contact_point_count);
                // Generate the collision manifold
                result_collisions[collision_index].collider1_index = index_1;
                result_collisions[collision_index].collider2_index = index_2;

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

    for(int i = 0; i < MAX_SCENE_COLLIDERS; i++) {
        if (col_contr->enabled_colliders[i] && (col_contr->collider_type[i] == OBB_COLLIDER || col_contr->collider_type[i] == AABB_COLLIDER)) {
            sMat44 model;
            model.set_scale(sVector3{col_contr->box_collider_sizes[i].x,
                                         col_contr->box_collider_sizes[i].y,
                                         col_contr->box_collider_sizes[i].z});

            model.rotate(&col_contr->box_collider_rotations[i]);

            model.set_position(col_contr->origin_points[i]);

            render_mesh(&col_contr->debug_renderer,
                        &model,
                        tracking,
                        eye_index,
                        true);
        }
    }
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
    controller->origin_points[index] = position;
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
    controller->origin_points[index] = position;
    controller->sphere_collider_radius[index] = radius;

    return index;
}

int CC_add_OBB_collider(sColliderController *controller,
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
    controller->collider_type[index] = OBB_COLLIDER;
    controller->origin_points[index] = position;
    controller->box_collider_sizes[index] = sVector3{width, heigth, depth};
    controller->box_collider_rotations[index] = rotation;

    return index;
}