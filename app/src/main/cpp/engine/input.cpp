//
// Created by jsmar on 06/06/2021.
//

#include "input.h"


void INPUT_get_controller_states(sControllerInput   *controler_state,
                                 ovrMobile          *ovr_instance,
                                 const double       predicted_display_time) {
    for (int controller_index = 0; controller_index < CONTROLLER_COUNT; controller_index++) {

        ovrInputCapabilityHeader input_header;

        if (vrapi_EnumerateInputDevices(ovr_instance, controller_index, &input_header) >= 0) {

            if (input_header.Type == ovrControllerType_TrackedRemote) {
                ovrInputTrackedRemoteCapabilities remote_capabilites;
                remote_capabilites.Header = input_header;

                if (vrapi_GetInputDeviceCapabilities(ovr_instance, &remote_capabilites.Header) >= 0) {
                    // The remote controller is conencted
                    ovrTracking controller_tracking;
                    ovrInputStateTrackedRemote ovr_controller_state;

                    ovr_controller_state.Header.ControllerType  = remote_capabilites.Header.Type;

                    if (vrapi_GetCurrentInputState(ovr_instance,
                                                   remote_capabilites.Header.DeviceID,
                                                   &ovr_controller_state.Header) >= 0) {
                        if (vrapi_GetInputTrackingState(ovr_instance,
                                                        remote_capabilites.Header.DeviceID,
                                                        predicted_display_time,
                                                        &controller_tracking) >= 0) {
                            controler_state->old_grip_triggers[controller_index] = controler_state->grip_triggers[controller_index];
                            controler_state->old_main_triggers[controller_index] = controler_state->main_triggers[controller_index];
                            controler_state->old_button_state = controler_state->button_state;

                            controler_state->grip_triggers[controller_index] = ovr_controller_state.GripTrigger;
                            controler_state->main_triggers[controller_index] = ovr_controller_state.IndexTrigger;

                            controler_state->button_state = 0;
                            if (ovr_controller_state.Buttons & ovrTouch_A) {
                                controler_state->button_state |= A_BUTTON;
                            }
                            if (ovr_controller_state.Buttons & ovrTouch_B) {
                                controler_state->button_state |= B_BUTTON;
                            }
                            if (ovr_controller_state.Buttons & ovrTouch_X) {
                                controler_state->button_state |= X_BUTTON;
                            }
                            if (ovr_controller_state.Buttons & ovrTouch_Y) {
                                controler_state->button_state |= Y_BUTTON;
                            }
                            if (ovr_controller_state.Buttons & ovrButton_Joystick) {
                                controler_state->button_state |= JOY_BUTTON;
                            }

                            controler_state->controller_positions[controller_index] = sVector3{
                                    controller_tracking.HeadPose.Pose.Position.x,
                                    controller_tracking.HeadPose.Pose.Position.y,
                                    controller_tracking.HeadPose.Pose.Position.z};
                            controler_state->controller_rotations[controller_index] = sQuaternion4{
                                    controller_tracking.HeadPose.Pose.Orientation.w,
                                    controller_tracking.HeadPose.Pose.Orientation.x,
                                    controller_tracking.HeadPose.Pose.Orientation.y,
                                    controller_tracking.HeadPose.Pose.Orientation.z};

                            controler_state->controller_rotations[controller_index] = controler_state->controller_rotations[controller_index].normalize();

                            controler_state->joysticks[controller_index] = sVector2{ovr_controller_state.Joystick.x, ovr_controller_state.Joystick.y};
                        }
                    }
                }
            } else {
                info("PROBLEM READING THE CONTROLLER STATE");
            }
        }
    }
}