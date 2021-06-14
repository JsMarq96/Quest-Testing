//
// Created by Juan S. Marquerie on 13/06/2021.
//

#include "test_stage.h"


void test_scene_update(sScene *curr_scene, const double frame_delta) {
    int left_hand_id = scene_fetch_object_id(curr_scene, "left_hand");
    int right_hand_id = scene_fetch_object_id(curr_scene, "right_hand");

    curr_scene->position[left_hand_id] = sVector3{1.0, -1.0f, 0.0f};
}