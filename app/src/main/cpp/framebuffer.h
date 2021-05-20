//
// Created by jsmar on 20/05/2021.
//

#ifndef QUEST_DEMO_FRAMEBUFFER_H
#define QUEST_DEMO_FRAMEBUFFER_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <stdlib.h>

#include "common.h"

struct framebuffer
{
    int swap_chain_index;
    int swap_chain_length;
    GLsizei width;
    GLsizei height;
    ovrTextureSwapChain* color_texture_swap_chain;
    GLuint* depth_renderbuffers;
    GLuint* framebuffers;
};


void
framebuffer_create(struct framebuffer* framebuffer,GLsizei width,GLsizei height);

void
framebuffer_destroy(struct framebuffer* framebuffer);


static const char*
gl_get_framebuffer_status_string(GLenum status)
{
    switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            return "GL_FRAMEBUFFER_UNDEFINED";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return "GL_FRAMEBUFFER_UNSUPPORTED";
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
        default:
            abort();
    }
}
#endif //QUEST_DEMO_FRAMEBUFFER_H
