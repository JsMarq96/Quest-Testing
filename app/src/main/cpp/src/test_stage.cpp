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

    curr_scene->position[right_hand_id] = input->controller_positions[RIGHT_CONTROLLER];
    curr_scene->position[left_hand_id] = input->controller_positions[LEFT_CONTROLLER];
}