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

#include "math.h"
#include "shader.h"
#include "display_layer.h"
#include "basic_renderer.h"
#include "basic_geometry.h"
#include "framebuffer.h"
#include "shader_program.h"
#include "common.h"
#include "frame_renderer.h"
#include "mesh.h"
#include "mesh_renderer.h"
#include "asset_extractor.h"

struct app
{
    ovrJava* java;
    struct egl egl;
    struct renderer renderer;
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
    renderer_create(&app->renderer,
                    vrapi_GetSystemPropertyInt(
                            java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH),
                    vrapi_GetSystemPropertyInt(
                            java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT));
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
    renderer_destroy(&app->renderer);
}

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

    sAssMan ass_man;
    init_asset_manager(&ass_man, java.Env, activity);
    info("Looked up root storage: %s", ass_man.root_dir);
    info("Looked up apk path: %s", ass_man.apk_dir);
    //ass_man.root_dir = "/data/data/app.upstairs.quest_sample_project/";

    if (!check_asset(&ass_man,"/res/raw/player_2.obj")) {
        extract_asset(&ass_man,
                      "res/raw/player_2.obj");
        info("Asset extracted");
    } else {
        info("Asset is found");
    }


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

    // Load example mesh
    sMesh player_mesh;

    char *res_size = "/data/data/app.upstairs.quest_sample_project/res/raw/player_2.obj";

    load_mesh(&player_mesh, res_size);

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

        // Check bottom presses, and loads the the exit message if necesary
        // No controller motion
        app_handle_input(&app);

        // Skip frame rendering if there is no a OVR runtime
        if (app.ovr == NULL) {
            continue;
        }
        app.frame_index++;

        // Predict the frame time, for accurate sensor prediction
        const double display_time =
                vrapi_GetPredictedDisplayTime(app.ovr, app.frame_index);

        // Get the prediccted tracking positions for controllersm and the projection
        // matrices for each eye
        ovrTracking2 tracking =
                vrapi_GetPredictedTracking2(app.ovr, display_time);

        const ovrLayerProjection2 layer =
                renderer_render_frame(&app.renderer, &tracking);

        const ovrLayerHeader2* layers[] = { &layer.Header };

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