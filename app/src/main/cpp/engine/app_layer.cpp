//
// Created by jsmar on 15/06/2021.
//

#include "app_layer.h"


void APP_LAYER_init(sAppLayer *app,
                    const android_app *android_app) {
    EGLint num_configs = 0;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLConfig *configs;
    EGLConfig found_config = NULL;
    static const EGLint CONFIG_ATTRIBS[] = {
            EGL_RED_SIZE,   8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16,
            EGL_SAMPLE_BUFFERS, 1, EGL_SAMPLES,  4, EGL_NONE
    };
    static const EGLint CONTEXT_ATTRIBS[] = { EGL_CONTEXT_CLIENT_VERSION, 3,
                                              EGL_NONE };
    static const EGLint SURFACE_ATTRIBS[] = {
            EGL_WIDTH, 16, EGL_HEIGHT, 16, EGL_NONE,
    };

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display,
                  NULL,
                  NULL);
    eglGetConfigs(display,
                  NULL,
                  0,
                  &num_configs);
    configs = (EGLConfig*) malloc(num_configs * sizeof(EGLConfig));
    eglGetConfigs(display,
                  configs,
                  num_configs,
                  &num_configs);

    // TF...?
    for (int i = 0; i < num_configs; i++) {
        EGLConfig config = configs[i];
        EGLint renderable_type = 0;
        eglGetConfigAttrib(display,
                           config,
                           EGL_RENDERABLE_TYPE,
                           &renderable_type);

        if ((renderable_type & EGL_OPENGL_ES3_BIT_KHR) == 0) {
            continue;
        }

        EGLint surface_type = 0;
        eglGetConfigAttrib(display, config, EGL_SURFACE_TYPE, &surface_type);

        if ((renderable_type & EGL_PBUFFER_BIT) == 0) {
            continue;
        }
        if ((renderable_type & EGL_WINDOW_BIT) == 0) {
            continue;
        }

        const EGLint* attrib = CONFIG_ATTRIBS;
        while (attrib[0] != EGL_NONE) {
            EGLint value = 0;
            eglGetConfigAttrib(display,
                               config,
                               attrib[0],
                               &value);
            if (value != attrib[1]) {
                break;
            }
            attrib += 2;
        }
        if (attrib[0] != EGL_NONE) {
            continue;
        }

        found_config = config;
        break;
    }

    EGLint msaaBuffers;
    eglGetConfigAttrib(display, found_config, EGL_SAMPLE_BUFFERS, &msaaBuffers);
    if (msaaBuffers == 0) {
        info("NO MSAA");
    } else {
        info("Enabled MSAA");
    }

    free(configs);

    context = eglCreateContext(display,
                               found_config,
                               EGL_NO_CONTEXT,
                               CONTEXT_ATTRIBS);

    surface = eglCreatePbufferSurface(display,
                                      found_config,
                                      SURFACE_ATTRIBS);

    eglMakeCurrent(display,
                   surface,
                   surface,
                   context);

    app->surface = surface;
    app->display = display;
    app->context = context;
}

void APP_LAYER_delete(sAppLayer *app) {
    eglMakeCurrent(app->display,
                   EGL_NO_SURFACE,
                   EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);

    eglDestroySurface(app->display,
                      app->surface);
    eglDestroyContext(app->display,
                      app->context);
    eglTerminate(app->display);
}