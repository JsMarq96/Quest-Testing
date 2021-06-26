//
// Created by jsmar on 07/06/2021.
//
#include "collision_controller.h"


//// LIFECYCLE FUNCTIONS

void CC_init(sColliderController *col_contr,
             bool                *enabled_objs,
             sVector3            *obj_positions,
             sQuaternion4        *obj_rotation) {
    memset(col_contr->object_entanglement, -1, MAX_SCENE_COLLIDERS);
    render_init_cube(&col_contr->debug_renderer, true);

    col_contr->position = obj_positions;
    col_contr->rotation = obj_rotation;
    col_contr->enabled = enabled_objs;
}

void CC_update(sColliderController    *col_contr,
               sCollisionManifold     *result_collisions,
               int                    *collision_count) {
    int enabled_collider_count = 0;
    int enabled_collider_indexing[MAX_SCENE_COLLIDERS] = {-1};

    // Move colliders if needed
    for(int i = 0; i <MAX_SCENE_COLLIDERS; i++) {
        if (!col_contr->enabled_colliders[i]) {
            continue;
        }
        if (col_contr->object_entanglement[i] == -1) {
            continue;
        }

        enabled_collider_indexing[enabled_collider_count++] = i;

        int obj_index = col_contr->object_entanglement[i];

        sMat44 rotate, scale;
        rotate.rotate(&col_contr->rotation[obj_index]);
        scale.set_scale(col_contr->scale[obj_index]);

        rotate.multiply(&scale);
        rotate.set_position(sVector3{col_contr->origin_points[i].x + col_contr->position[obj_index].x,
                                     col_contr->origin_points[i].y + col_contr->position[obj_index].y,
                                     col_contr->origin_points[i].z + col_contr->position[obj_index].z });


        col_contr->box_transform[i] = rotate;
    }

    // Detect collisions
    int collision_index = 0;
    // TODO: Naive bubblesort, fine for this stage, but could need some optimization on the future
    for(int i = 0; i < enabled_collider_count; i++) {
        for(int j = i+1; j < enabled_collider_count; j++) {
            int index_1 = enabled_collider_indexing[i];
            int index_2 = enabled_collider_indexing[j];

            if (SAT_OBB_v_OBB(col_contr->box_transform[index_1],
                              col_contr->rotation[index_1],
                              col_contr->box_transform[index_2],
                              col_contr->rotation[index_2],
                              &result_collisions[collision_index])) {

                info("COLLISION BEIBIIII %d %d %d", index_1, index_2, result_collisions[collision_index].contact_point_count);
                // Generate the collision manifold
                result_collisions[collision_index].obj1_index = index_1;
                result_collisions[collision_index].obj2_index = index_2;

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
        if (col_contr->enabled_colliders[i] && (col_contr->collider_type[i] == OBB_COLLIDER)) {
            render_mesh(&col_contr->debug_renderer,
                        &col_contr->box_transform[i],
                        tracking,
                        eye_index,
                        true);
        }
    }
}

//// COLLIDER MANAGING FUNCTIONS

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
    controller->scale[index] = sVector3{width, heigth, width};

    sMat44 transform;

    transform.set_position(position);
    transform.set_scale(controller->scale[index]);
    transform.rotate(&rotation);

    controller->box_transform[index] = transform;

    return index;
}