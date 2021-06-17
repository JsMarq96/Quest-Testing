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
                       const double             frame_delta) {
    int left_hand_id = scene_fetch_object_id(curr_scene, "left_hand");
    int right_hand_id = scene_fetch_object_id(curr_scene, "right_hand");
    int ship_1_id = scene_fetch_object_id(curr_scene, "ship_1");
    int ship_2_id = scene_fetch_object_id(curr_scene, "ship_2");

    curr_scene->position[right_hand_id] = input->controller_positions[RIGHT_CONTROLLER];
    curr_scene->position[left_hand_id] = input->controller_positions[LEFT_CONTROLLER];
    curr_scene->rotation[right_hand_id] = input->controller_rotations[RIGHT_CONTROLLER];
    curr_scene->rotation[left_hand_id] = input->controller_rotations[LEFT_CONTROLLER];
    curr_scene->rotation[ship_2_id] = input->controller_rotations[LEFT_CONTROLLER];
    curr_scene->rotation[ship_1_id] = input->controller_rotations[RIGHT_CONTROLLER];

    info("CONTROLLER %f %f %f %f",
         input->controller_rotations[RIGHT_CONTROLLER].x,
         input->controller_rotations[RIGHT_CONTROLLER].y,
         input->controller_rotations[RIGHT_CONTROLLER].z,
         input->controller_rotations[RIGHT_CONTROLLER].w);
}