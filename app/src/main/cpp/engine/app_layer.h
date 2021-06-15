//
// Created by jsmar on 15/06/2021.
//

#ifndef QUEST_DEMO_APP_LAYER_H
#define QUEST_DEMO_APP_LAYER_H

#include <android_native_app_glue.h>
#include <android/window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdlib.h>

#include "../utils/common.h"

struct sAppLayer {
    bool app_resumed        = true;
    ANativeWindow* window   = NULL;
    long long frame_index   = 0;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
};

void APP_LAYER_init(sAppLayer *app,
                    const android_app *android_app);

void APP_LAYER_delete(sAppLayer *app);

#endif //QUEST_DEMO_APP_LAYER_H
