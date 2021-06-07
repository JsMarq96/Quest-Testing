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

                if (vrapi_GetInputDeviceCapabilities(ovr_instance, &remote_capabilites.Header) >=
                    0) {
                    // The remote controller is conencted
                    ovrTracking controller_tracking;
                    if (vrapi_GetInputTrackingState(ovr_instance,
                                                    remote_capabilites.Header.DeviceID,
                                                    predicted_display_time,
                                                    &controller_tracking) >= 0) {

                        controler_state->controller_positions[controller_index] = sVector3{
                                controller_tracking.HeadPose.Pose.Position.x,
                                controller_tracking.HeadPose.Pose.Position.y,
                                controller_tracking.HeadPose.Pose.Position.z};
                        controler_state->controller_rotations[controller_index] = sQuaternion4{
                                controller_tracking.HeadPose.Pose.Orientation.w,
                                controller_tracking.HeadPose.Pose.Orientation.x,
                                controller_tracking.HeadPose.Pose.Orientation.y,
                                controller_tracking.HeadPose.Pose.Orientation.z};
                        //controller_tracking.HeadPose.Pose.
                    } else {
                        info("PROBLEM READING THE CONTROLLER STATE");
                    }
                }
            } else {
                info("PROBLEM READING THE CONTROLLER STATE");
            }
        }
    }
}