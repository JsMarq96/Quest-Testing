//
// Created by jsmar on 06/06/2021.
//

#ifndef QUEST_DEMO_INPUT_H
#define QUEST_DEMO_INPUT_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Input.h>
#include <GLES3/gl3.h>

#include "math.h"
#include "common.h"

enum eControllerIds : int {
    LEFT_CONTROLLER = 0,
    RIGHT_CONTROLLER,
    CONTROLLER_COUNT
};

struct sControllerInput {
    sVector3      controller_positions [CONTROLLER_COUNT]  = { sVector3{0.f, 0.f, 0.f} };
    sQuaternion4  controller_rotations [CONTROLLER_COUNT]  = { sQuaternion4{0.f, 0.f, 0.f, 1.0f} };
    sVector3      controller_speeds    [CONTROLLER_COUNT]  = { sVector3{0.f, 0.f, 0.f} };

    // TODO: Button tracking
};

void INPUT_get_controller_states(sControllerInput   *controler_state,
                                 ovrMobile          *ovr_instance,
                                 const double       predicted_display_time);

#endif //QUEST_DEMO_INPUT_H
