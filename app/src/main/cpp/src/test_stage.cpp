//
// Created by Juan S. Marquerie on 13/06/2021.
//

#include "test_stage.h"

#include <android/log.h>
#include "../utils/common.h"

void test_scene_start(sScene   *curr_scene) {
    int left_hand_id = scene_fetch_object_id(curr_scene, "left_hand");
    scene_add_collider(curr_scene,
                       sVector3{},
                       sVector3{0.05f, 0.05f, 0.05f},
                       sQuaternion4{});
}

void test_scene_update(sScene                  *curr_scene,
                       const sControllerInput  *input,
                       const sCollisionManifold *manifolds,
                       const int                manifold_count,
                       const double             frame_delta) {
    int left_hand_id = scene_fetch_object_id(curr_scene, "left_hand");
    int right_hand_id = scene_fetch_object_id(curr_scene, "right_hand");
    int ship_1_id = scene_fetch_object_id(curr_scene, "ship_1");
    int ship_2_id = scene_fetch_object_id(curr_scene, "ship_2");

    curr_scene->position[right_hand_id] = input->controller_positions[RIGHT_CONTROLLER];
    curr_scene->position[left_hand_id] = input->controller_positions[LEFT_CONTROLLER];
    curr_scene->rotation[right_hand_id] = input->controller_rotations[RIGHT_CONTROLLER];
    curr_scene->rotation[left_hand_id] = input->controller_rotations[LEFT_CONTROLLER];
    /*curr_scene->rotation[ship_2_id] = input->controller_rotations[LEFT_CONTROLLER];
    curr_scene->rotation[ship_1_id] = input->controller_rotations[RIGHT_CONTROLLER];*/

    sVector3 collider_pos = curr_scene->collision_controller.origin_points[manifolds[0].collider2_index];

    if (manifold_count > 0) {
        sVector3 col_normal = manifolds[0].collision_normal;
        float max_depth = 0.0f;

        for (int i = 0; i < manifolds[0].contact_point_count; i ++) {
            max_depth = MIN(manifolds[0].points_depth[i], max_depth);
        }

        col_normal.x *= max_depth * -1.0f * frame_delta;
        col_normal.y *= max_depth * -1.0f * frame_delta;
        col_normal.z *= max_depth * -1.0f * frame_delta;

        collider_pos.x += col_normal.x;
        collider_pos.y += col_normal.y;
        collider_pos.z += col_normal.z;

        curr_scene->collision_controller.origin_points[manifolds[0].collider2_index] = collider_pos;
    }
}