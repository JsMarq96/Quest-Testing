//
// Created by jsmar on 06/06/2021.
//

#ifndef QUEST_DEMO_INPUT_H
#define QUEST_DEMO_INPUT_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Input.h>
#include <GLES3/gl3.h>

#include "../utils/math.h"
#include "../utils/common.h"

#define CONTR_SWAPCHAIN 2

enum eControllerIds : int {
    LEFT_CONTROLLER = 0,
    RIGHT_CONTROLLER,
    CONTROLLER_COUNT
};

enum eControllerButtonState : unsigned char {
    A_BUTTON        = 0b0000001,
    B_BUTTON        = 0b0000010,
    X_BUTTON        = 0b0000100,
    Y_BUTTON        = 0b0001000,
    JOY_BUTTON      = 0b0010000,
    OCULUS_BUTTON   = 0b0100000,
    MENU_BUTTON     = 0b1000000
};

struct sControllerInput {
    /// CONTROLLER POSITION
    sVector3      controller_positions [CONTROLLER_COUNT]  = { sVector3{0.f, 0.f, 0.f} };
    sQuaternion4  controller_rotations [CONTROLLER_COUNT]  = { sQuaternion4{0.f, 0.f, 0.f, 1.0f} };
    sVector3      controller_speeds    [CONTROLLER_COUNT]  = { sVector3{0.f, 0.f, 0.f} };

    /// BUTTON STATE
    float         grip_triggers        [CONTROLLER_COUNT]  = { 0.0f };
    float         main_triggers        [CONTROLLER_COUNT]  = { 0.0f };
    unsigned char button_state                             = { 0 };
    float         old_grip_triggers    [CONTROLLER_COUNT]  = { 0.0f };
    float         old_main_triggers    [CONTROLLER_COUNT]  = { 0.0f };
    unsigned char old_button_state                             = { 0 };

    /// JOYSTICKS
    sVector2      joysticks            [CONTROLLER_COUNT] = { sVector2{0.0f, 0.0f} };
};

void INPUT_get_controller_states(sControllerInput   *controler_state,
                                 ovrMobile          *ovr_instance,
                                 const double       predicted_display_time);

inline bool INPUT_check_button_press(const sControllerInput *contr_state, const eControllerButtonState state) {
    return (contr_state->button_state & state) && !(contr_state->old_button_state & state);
}

inline bool INPUT_check_button_release(const sControllerInput *contr_state, const eControllerButtonState state) {
    return !(contr_state->button_state & state) && (contr_state->old_button_state & state);
}

#endif //QUEST_DEMO_INPUT_H
