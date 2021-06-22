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

#include "../engine/app_layer.h"
#include "../engine/mesh_renderer.h"
#include "../engine/frame_renderer.h"
#include "../engine/skybox_renderer.h"
#include "../engine/batch_mesh_renderer.h"
#include "../engine/raw_shaders.h"
#include "../engine/mesh.h"
#include "../engine/texture.h"
#include "../engine/material.h"
#include "../engine/input.h"
#include "../engine/engine.h"

#include "../utils/common.h"
#include "../utils/asset_manager.h"
#include "../engine/shader.h"
#include "../utils/math.h"

#include "test_stage.h"

static const int CPU_LEVEL = 2;
static const int GPU_LEVEL = 3;

static void
app_on_cmd(struct android_app* android_app, int32_t cmd)
{
    struct sAppLayer* app = (sAppLayer*) android_app->userData;
    switch (cmd) {
        case APP_CMD_START:
            info("onStart()");
            break;
        case APP_CMD_RESUME:
            info("onResume()");
            app->app_resumed = true;
            break;
        case APP_CMD_PAUSE:
            info("onPause()");
            app->app_resumed = false;
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

    sAppLayer app;

    android_app->userData= &app;
    android_app->onAppCmd = app_on_cmd;

    // Init app, and EGL context
    APP_LAYER_init(&app,
                   android_app);

    info("initialize vr api");
    const ovrInitParms init_parms = vrapi_DefaultInitParms(&java);
    if (vrapi_Initialize(&init_parms) != VRAPI_INITIALIZE_SUCCESS) {
        info("can't initialize vr api");
        exit(EXIT_FAILURE);
    }

    AM_init(&ass_man_instance, java.Env, android_app->activity);
    info("Looked up root storage: %s", ass_man_instance.root_asset_dir);
    info("Looked up apk path: %s", ass_man_instance.apk_dir);

    sFramebuffers framebuffers;

    framebuffers_init(&framebuffers,
                      vrapi_GetSystemPropertyInt(&java,
                                                 VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH),
                      vrapi_GetSystemPropertyInt(&java,
                                                 VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT));

    sFrameRenderer frame_renderer;
    init_frame_renderer(&frame_renderer,
                        vrapi_GetSystemPropertyInt(&java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH),
                        vrapi_GetSystemPropertyInt(&java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT));

    /// LOAD THE
    sEngineInstance engine;
    ENGINE_add_scene(&engine,
                     "res/raw/test_scene.scene");

    sScene *curr_scene = &engine.curr_scene->curr_scene;

    int left_hand_id = scene_fetch_object_id(curr_scene, "left_hand");
    int hand_col_id =  scene_add_collider(curr_scene,
                       sVector3{},
                       sVector3{0.05f, 0.05f, 0.05f},
                       sQuaternion4{1.0f,0.0f,0.0f, 0.0f});

    scene_attach_collider_to_object(curr_scene,
                                    hand_col_id,
                                    left_hand_id,
                                    sVector3{0.0f});

    scene_add_collider(curr_scene,
                       sVector3{-1.0f, 0.5f, 0.f},
                       sVector3{0.5f, 0.5f, 0.5f},
                       sQuaternion4{0.89f,0.373f,0.095f, -0.2290f});

    curr_scene->scene_update = &test_scene_update;

    ovrMobile *ovr = NULL;

    while(!android_app->destroyRequested) {
        ANativeActivity *activity = android_app->activity;

        for(;;) {
            int events = 0;
            struct android_poll_source* source = NULL;
            // Check for system events with the androind_native_app_glue, based on the
            // state of the app
            if (ALooper_pollAll(android_app->destroyRequested || ovr != NULL ? 0 : -1,
                                NULL,
                                &events,
                                (void**)&source) < 0) {
                break;
            }

            // Process the detected event
            if (source != NULL) {
                source->process(android_app, source);
            }

            if (app.app_resumed && app.window != NULL) {
                if (ovr == NULL) {
                    ovrModeParms params = vrapi_DefaultModeParms(&java);
                    params.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
                    params.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
                    params.Display = (size_t) app.display;
                    params.WindowSurface = (size_t) app.window;
                    params.ShareContext = (size_t) app.context;
                    info("Enter VR mode");
                    ovr = vrapi_EnterVrMode(&params);

                    vrapi_SetTrackingSpace(ovr, VRAPI_TRACKING_SPACE_LOCAL);
                    vrapi_SetDisplayRefreshRate(ovr, 120.0f );
                    vrapi_SetClockLevels(ovr, CPU_LEVEL, GPU_LEVEL);
                }
            } else {
                if (ovr != NULL) {
                    info("Leave VR mode");
                    vrapi_LeaveVrMode(ovr);
                    ovr = NULL;
                }
            }
        }
        app.frame_index++;

        // Predict the frame time, for accurate sensor prediction
        const double display_time = vrapi_GetPredictedDisplayTime(ovr,
                                                                  app.frame_index);
        const ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovr,
                                                                  display_time);

        /// FRAME GENRATION START
        sControllerInput cont_input;
        INPUT_get_controller_states(&cont_input,
                                    ovr,
                                    display_time);

        scene_update(curr_scene, &cont_input, display_time);

        render_frame(&frame_renderer,
                     curr_scene,
                     &tracking);

        /// FRAME GENRATION END

        // Creates a world view layer and configures it
        ovrLayerProjection2 layer = vrapi_DefaultLayerProjection2();
        layer.Header.Flags |=
                VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;
        layer.HeadPose = tracking.HeadPose;

        const ovrLayerHeader2* layers[] = { &frame_renderer.frame_layer.Header };

        ovrSubmitFrameDescription2 frame;
        frame.Flags = 0;
        frame.SwapInterval = 1;
        frame.FrameIndex = app.frame_index;
        frame.DisplayTime = display_time;
        frame.LayerCount = 1;
        frame.Layers = layers;

        vrapi_SubmitFrame2(ovr, &frame);
    }

    info("shut down vr api");
    vrapi_Shutdown();

    APP_LAYER_delete(&app);

    info("detach current thread");
    (java.Vm)->DetachCurrentThread();
}