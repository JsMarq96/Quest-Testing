//
// Created by jsmar on 20/05/2021.
//

#ifndef QUEST_DEMO_DISPLAY_LAYER_H
#define QUEST_DEMO_DISPLAY_LAYER_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdlib.h>

#include "../utils/common.h"

struct egl
{
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
};

 void
egl_create(struct egl* egl);

 void
egl_destroy(struct egl* egl);

static const char*
egl_get_error_string(EGLint error)
{
    switch (error) {
        case EGL_SUCCESS:
            return "EGL_SUCCESS";
        case EGL_NOT_INITIALIZED:
            return "EGL_NOT_INITIALIZED";
        case EGL_BAD_ACCESS:
            return "EGL_BAD_ACCESS";
        case EGL_BAD_ALLOC:
            return "EGL_BAD_ALLOC";
        case EGL_BAD_ATTRIBUTE:
            return "EGL_BAD_ATTRIBUTE";
        case EGL_BAD_CONTEXT:
            return "EGL_BAD_CONTEXT";
        case EGL_BAD_CONFIG:
            return "EGL_BAD_CONFIG";
        case EGL_BAD_CURRENT_SURFACE:
            return "EGL_BAD_CURRENT_SURFACE";
        case EGL_BAD_DISPLAY:
            return "EGL_BAD_DISPLAY";
        case EGL_BAD_SURFACE:
            return "EGL_BAD_SURFACE";
        case EGL_BAD_MATCH:
            return "EGL_BAD_MATCH";
        case EGL_BAD_PARAMETER:
            return "EGL_BAD_PARAMETER";
        case EGL_BAD_NATIVE_PIXMAP:
            return "EGL_BAD_NATIVE_PIXMAP";
        case EGL_BAD_NATIVE_WINDOW:
            return "EGL_BAD_NATIVE_WINDOW";
        case EGL_CONTEXT_LOST:
            return "EGL_CONTEXT_LOST";
        default:
            abort();
    }
}

#endif //QUEST_DEMO_DISPLAY_LAYER_H