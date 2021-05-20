//
// Created by jsmar on 20/05/2021.
//

#include "display_layer.h"

 void
egl_create(struct egl* egl)
{
    info("get EGL display");
    egl->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl->display == EGL_NO_DISPLAY) {
        error("can't get EGL display: %s", egl_get_error_string(eglGetError()));
        exit(EXIT_FAILURE);
    }

    info("initialize EGL display");
    if (eglInitialize(egl->display, NULL, NULL) == EGL_FALSE) {
        error("can't initialize EGL display: %s",
              egl_get_error_string(eglGetError()));
        exit(EXIT_FAILURE);
    }

    info("get number of EGL configs");
    EGLint num_configs = 0;
    if (eglGetConfigs(egl->display, NULL, 0, &num_configs) == EGL_FALSE) {
        error("can't get number of EGL configs: %s",
              egl_get_error_string(eglGetError()));
        exit(EXIT_FAILURE);
    }

    info("allocate EGL configs");
    EGLConfig *configs = (EGLConfig*) malloc(num_configs * sizeof(EGLConfig));
    if (configs == NULL) {
        error("cant allocate EGL configs: %s",
              egl_get_error_string(eglGetError()));
        exit(EXIT_FAILURE);
    }

    info("get EGL configs");
    if (eglGetConfigs(egl->display, configs, num_configs, &num_configs) ==
        EGL_FALSE) {
        error("can't get EGL configs: %s", egl_get_error_string(eglGetError()));
        exit(EXIT_FAILURE);
    }

    info("choose EGL config");
    static const EGLint CONFIG_ATTRIBS[] = {
            EGL_RED_SIZE,   8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE,    8,
            EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 0, EGL_STENCIL_SIZE, 0,
            EGL_SAMPLES,    0, EGL_NONE,
    };
    EGLConfig found_config = NULL;
    for (int i = 0; i < num_configs; ++i) {
        EGLConfig config = configs[i];

        info("get EGL config renderable type");
        EGLint renderable_type = 0;
        if (eglGetConfigAttrib(egl->display, config, EGL_RENDERABLE_TYPE,
                               &renderable_type) == EGL_FALSE) {
            error("can't get EGL config renderable type: %s",
                  egl_get_error_string(eglGetError()));
            exit(EXIT_FAILURE);
        }
        if ((renderable_type & EGL_OPENGL_ES3_BIT_KHR) == 0) {
            continue;
        }

        info("get EGL config surface type");
        EGLint surface_type = 0;
        if (eglGetConfigAttrib(egl->display, config, EGL_SURFACE_TYPE,
                               &surface_type) == EGL_FALSE) {
            error("can't get EGL config surface type: %s",
                  egl_get_error_string(eglGetError()));
            exit(EXIT_FAILURE);
        }
        if ((renderable_type & EGL_PBUFFER_BIT) == 0) {
            continue;
        }
        if ((renderable_type & EGL_WINDOW_BIT) == 0) {
            continue;
        }

        const EGLint* attrib = CONFIG_ATTRIBS;
        while (attrib[0] != EGL_NONE) {
            info("get EGL config attrib");
            EGLint value = 0;
            if (eglGetConfigAttrib(egl->display, config, attrib[0], &value) ==
                EGL_FALSE) {
                error("can't get EGL config attrib: %s",
                      egl_get_error_string(eglGetError()));
                exit(EXIT_FAILURE);
            }
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
    if (found_config == NULL) {
        error("can't choose EGL config");
        exit(EXIT_FAILURE);
    }

    info("free EGL configs");
    free(configs);

    info("create EGL context");
    static const EGLint CONTEXT_ATTRIBS[] = { EGL_CONTEXT_CLIENT_VERSION, 3,
                                              EGL_NONE };
    egl->context = eglCreateContext(egl->display, found_config, EGL_NO_CONTEXT,
                                    CONTEXT_ATTRIBS);
    if (egl->context == EGL_NO_CONTEXT) {
        error("can't create EGL context: %s",
              egl_get_error_string(eglGetError()));
        exit(EXIT_FAILURE);
    }

    info("create EGL surface");
    static const EGLint SURFACE_ATTRIBS[] = {
            EGL_WIDTH, 16, EGL_HEIGHT, 16, EGL_NONE,
    };
    egl->surface =
            eglCreatePbufferSurface(egl->display, found_config, SURFACE_ATTRIBS);
    if (egl->surface == EGL_NO_SURFACE) {
        error("can't create EGL pixel buffer surface: %s",
              egl_get_error_string(eglGetError()));
        exit(EXIT_FAILURE);
    }

    info("make EGL context current");
    if (eglMakeCurrent(egl->display, egl->surface, egl->surface,
                       egl->context) == EGL_FALSE) {
        error("can't make EGL context current: %s",
              egl_get_error_string(eglGetError()));
    }
}

 void
egl_destroy(struct egl* egl)
{
    info("make EGL context no longer current");
    eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);

    info("destroy EGL surface");
    eglDestroySurface(egl->display, egl->surface);

    info("destroy EGL context");
    eglDestroyContext(egl->display, egl->context);

    info("terminate EGL display");
    eglTerminate(egl->display);
}