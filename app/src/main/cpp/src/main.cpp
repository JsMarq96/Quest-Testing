#include <VrApi.h>
#include <VrApi_Helpers.h>
#include <VrApi_Input.h>
#include <VrApi_SystemUtils.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <android/log.h>
#include <android/window.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <android/asset_manager.h>

#include <zip.h>

#include "../utils/math.h"
#include "../engine/shader.h"
#include "../engine/display_layer.h"
#include "../utils/common.h"
#include "../engine/frame_renderer.h"
#include "../engine/mesh.h"
#include "../engine/mesh_renderer.h"
#include "../utils/asset_manager.h"
#include "../engine/texture.h"
#include "../engine/skybox_renderer.h"
#include "../engine/material.h"
#include "../engine/batch_mesh_renderer.h"
#include "../engine/input.h"

#include "../engine/raw_shaders.h"

#include "../engine/engine.h"

#include "test_stage.h"

struct app
{
    ovrJava* java;
    struct egl egl;
    bool resumed;
    ANativeWindow* window;
    ovrMobile* ovr;
    bool back_button_down_previous_frame;
    uint64_t frame_index;
};

static const int CPU_LEVEL = 2;
static const int GPU_LEVEL = 3;

static void
app_on_cmd(struct android_app* android_app, int32_t cmd)
{
    struct app* app = (struct app*)android_app->userData;
    switch (cmd) {
        case APP_CMD_START:
            info("onStart()");
            break;
        case APP_CMD_RESUME:
            info("onResume()");
            app->resumed = true;
            break;
        case APP_CMD_PAUSE:
            info("onPause()");
            app->resumed = false;
            break;
        case APP_CMD_STOP:
            info("onStop()");
            break;
        case APP_CMD_DESTROY:
            info("onDestroy()");
            app->window = NULL;
            break;
        case APP_CMD_INIT_WINDOW:
            info("surfaceCreated()");
            app->window = android_app->window;
            break;
        case APP_CMD_TERM_WINDOW:
            info("surfaceDestroyed()");
            app->window = NULL;
            break;
        default:
            break;
    }
}

static void
app_update_vr_mode(struct app* app)
{
    if (app->resumed && app->window != NULL) {
        if (app->ovr == NULL) {
            ovrModeParms mode_parms = vrapi_DefaultModeParms(app->java);
            mode_parms.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
            mode_parms.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
            mode_parms.Display = (size_t)app->egl.display;
            mode_parms.WindowSurface = (size_t)app->window;
            mode_parms.ShareContext = (size_t)app->egl.context;

            info("enter vr mode");
            app->ovr = vrapi_EnterVrMode(&mode_parms);
            if (app->ovr == NULL) {
                error("can't enter vr mode");
                exit(EXIT_FAILURE);
            }

            vrapi_SetClockLevels(app->ovr, CPU_LEVEL, GPU_LEVEL);
        }
    } else {
        if (app->ovr != NULL) {
            info("leave vr mode");
            vrapi_LeaveVrMode(app->ovr);
            app->ovr = NULL;
        }
    }
}

static void
app_handle_input(struct app* app)
{
    bool back_button_down_current_frame = false;

    int i = 0;
    ovrInputCapabilityHeader capability;
    while (vrapi_EnumerateInputDevices(app->ovr, i, &capability) >= 0) {
        if (capability.Type == ovrControllerType_TrackedRemote) {
            ovrInputStateTrackedRemote input_state;
            input_state.Header.ControllerType = ovrControllerType_TrackedRemote;
            if (vrapi_GetCurrentInputState(app->ovr, capability.DeviceID,
                                           &input_state.Header) == ovrSuccess) {
                back_button_down_current_frame |=
                        input_state.Buttons & ovrButton_Back;
                back_button_down_current_frame |=
                        input_state.Buttons & ovrButton_B;
                back_button_down_current_frame |=
                        input_state.Buttons & ovrButton_Y;
            }

        }
        ++i;
    }

    if (app->back_button_down_previous_frame &&
        !back_button_down_current_frame) {
        vrapi_ShowSystemUI(app->java, VRAPI_SYS_UI_CONFIRM_QUIT_MENU);
    }
    app->back_button_down_previous_frame = back_button_down_current_frame;
}

static void
app_create(struct app* app, ovrJava* java)
{
    app->java = java;
    egl_create(&app->egl);
    app->resumed = false;
    app->window = NULL;
    app->ovr = NULL;
    app->back_button_down_previous_frame = false;
    app->frame_index = 0;
}

static void
app_destroy(struct app* app)
{
    egl_destroy(&app->egl);
}


sAssMan ass_man_instance;


void
android_main(struct android_app* android_app)
{
    ANativeActivity_setWindowFlags(android_app->activity,
                                   AWINDOW_FLAG_KEEP_SCREEN_ON,
                                   0);

    info("attach current thread");

    ovrJava java;
    java.Vm = android_app->activity->vm;
    (java.Vm)->AttachCurrentThread(&java.Env, NULL);
    java.ActivityObject = android_app->activity->clazz;

    ANativeActivity* activity = android_app->activity;

    info("initialize vr api");
    const ovrInitParms init_parms = vrapi_DefaultInitParms(&java);
    if (vrapi_Initialize(&init_parms) != VRAPI_INITIALIZE_SUCCESS) {
        info("can't initialize vr api");
        exit(EXIT_FAILURE);
    }

    struct app app;
    // Intializze EGL and the renderer
    app_create(&app, &java);

    android_app->userData = &app;
    android_app->onAppCmd = app_on_cmd;

    vrapi_SetDisplayRefreshRate( app.ovr, 120.0f );

    AM_init(&ass_man_instance, java.Env, activity);
    info("Looked up root storage: %s", ass_man_instance.root_asset_dir);
    info("Looked up apk path: %s", ass_man_instance.apk_dir);

    /*int test_collider = scene_add_collider(&default_scene,
                                           sVector3{0.0f, 0.f, 0.0f},
                                           sVector3{0.015f, .015f, .015f});

    CC_add_OBB_collider(&default_scene.collision_controller,
                         sVector3{-1.0f, 0.f, 0.0f},
                         sQuaternion4{1.0f, 0.0f, 0.f, 0.f},
                         0.5f, 0.5f, 0.5f);



    scene_attach_collider_to_object(&default_scene,
                                    test_collider,
                                    right_hand_id,
                                    sVector3{0.0f, -0.05f, 0.0f});*/

    // Create frame renderer
    sFrameRenderer frame_renderer;
    init_frame_renderer(&frame_renderer,
                        vrapi_GetSystemPropertyInt(&java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH),
                        vrapi_GetSystemPropertyInt(&java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT));

    sEngineInstance engine;

    ENGINE_add_scene(&engine,
                     "res/raw/test_scene.scene");

    sScene *curr_scene = &engine.curr_scene->curr_scene;

    curr_scene->scene_update = &test_scene_update;

    info("right hand id %i", KVS_get_int(&curr_scene->obj_index_storage, "right_hand", strlen("right_hand")));
    info("left hand id %i", KVS_get_int(&curr_scene->obj_index_storage, "left_hand", strlen("left_hand`")));
    info("ship 2 id %i", KVS_get_int(&curr_scene->obj_index_storage, "ship_2", strlen("ship_2")));

    while (!android_app->destroyRequested) {

        // Check for events, mainly for shutting down or initializing the OVR runtime
        for (;;) {
            int events = 0;
            struct android_poll_source* source = NULL;
            // Check for system events with the androind_native_app_glue, based on the
            // state of the app
            if (ALooper_pollAll(android_app->destroyRequested || app.ovr != NULL ? 0 : -1,
                                NULL,
                                &events,
                                (void**)&source) < 0) {
                break;
            }

            // Process the detected event
            if (source != NULL) {
                source->process(android_app, source);
            }

            // Switch between VR mode and non VR mode
            app_update_vr_mode(&app);
        }
        // Skip frame rendering if there is no a OVR runtime
        if (app.ovr == NULL) {
            continue;
        }
        app.frame_index++;

        // Predict the frame time, for accurate sensor prediction
        const double display_time =
                vrapi_GetPredictedDisplayTime(app.ovr, app.frame_index);

        /// INPUT MANAGING
        sControllerInput cont_input;
        INPUT_get_controller_states(&cont_input, app.ovr, display_time);

        /*default_scene.position[right_hand_id] = cont_input.controller_positions[RIGHT_CONTROLLER];
        default_scene.position[left_hand_id] = cont_input.controller_positions[LEFT_CONTROLLER];
        default_scene.rotation[right_hand_id] = cont_input.controller_rotations[RIGHT_CONTROLLER];
        default_scene.rotation[left_hand_id] = cont_input.controller_rotations[LEFT_CONTROLLER];

        default_scene.rotation[ship_1] = cont_input.controller_rotations[LEFT_CONTROLLER];
        default_scene.rotation[ship_2] = cont_input.controller_rotations[RIGHT_CONTROLLER];


        ;*/

        scene_update(curr_scene, display_time);


        // Get the prediccted tracking positions for the headset and the projection
        // matrices for each eye
        ovrTracking2 tracking =
                vrapi_GetPredictedTracking2(app.ovr, display_time);



        // Creates a world view layer and configures it
        ovrLayerProjection2 layer = vrapi_DefaultLayerProjection2();
        layer.Header.Flags |=
                VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;
        layer.HeadPose = tracking.HeadPose;

        /// FRAME RENDERER
        render_frame(&frame_renderer,
                     curr_scene,
                     &tracking);

        const ovrLayerHeader2* layers[] = { &frame_renderer.frame_layer.Header };

        ovrSubmitFrameDescription2 frame;
        frame.Flags = 0;
        frame.SwapInterval = 1;
        frame.FrameIndex = app.frame_index;
        frame.DisplayTime = display_time;
        frame.LayerCount = 1;
        frame.Layers = layers;

        vrapi_SubmitFrame2(app.ovr, &frame);
    }

    app_destroy(&app);

    info("shut down vr api");
    vrapi_Shutdown();

    info("detach current thread");
    (java.Vm)->DetachCurrentThread();
}