//
// Created by jsmar on 20/05/2021.
//

#include "framebuffer.h"

void
framebuffer_create(struct framebuffer* framebuffer,GLsizei width,
                   GLsizei height)
{
    framebuffer->swap_chain_index = 0;
    framebuffer->width = width;
    framebuffer->height = height;

    info("create color texture swap chain");
    framebuffer->color_texture_swap_chain = vrapi_CreateTextureSwapChain3(VRAPI_TEXTURE_TYPE_2D,
                                                                          GL_RGBA8,
                                                                          width,
                                                                          height,
                                                                          1,
                                                                          3);
    if (framebuffer->color_texture_swap_chain == NULL) {
        error("can't create color texture swap chain");
        exit(EXIT_FAILURE);
    }

    framebuffer->swap_chain_length =
            vrapi_GetTextureSwapChainLength(framebuffer->color_texture_swap_chain);

    info("allocate depth renderbuffers");
    framebuffer->depth_renderbuffers = (GLuint*)
            malloc(framebuffer->swap_chain_length * sizeof(GLuint));
    if (framebuffer->depth_renderbuffers == NULL) {
        error("can't allocate depth renderbuffers");
        exit(EXIT_FAILURE);
    }

    info("allocate framebuffers");
    framebuffer->framebuffers = (GLuint*)
            malloc(framebuffer->swap_chain_length * sizeof(GLuint));
    if (framebuffer->framebuffers == NULL) {
        error("can't allocate framebuffers");
        exit(EXIT_FAILURE);
    }

    glGenRenderbuffers(framebuffer->swap_chain_length,
                       framebuffer->depth_renderbuffers);
    glGenFramebuffers(framebuffer->swap_chain_length,
                      framebuffer->framebuffers);

    // Configure framebuffers's texture
    for (int i = 0; i < framebuffer->swap_chain_length; ++i) {
        info("create color texture %d", i);
        GLuint color_texture = vrapi_GetTextureSwapChainHandle(
                framebuffer->color_texture_swap_chain, i);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        info("create depth renderbuffer %d", i);
        glBindRenderbuffer(GL_RENDERBUFFER,
                           framebuffer->depth_renderbuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width,
                              height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        info("create framebuffer %d", i);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->framebuffers[i]);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, color_texture, 0);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  framebuffer->depth_renderbuffers[i]);
        GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            error("can't create framebuffer %d: %s", i,
                  gl_get_framebuffer_status_string(status));
            exit(EXIT_FAILURE);
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

void
framebuffer_destroy(struct framebuffer* framebuffer)
{
    info("destroy framebuffers");
    glDeleteFramebuffers(framebuffer->swap_chain_length,
                         framebuffer->framebuffers);

    info("destroy depth renderbuffers");
    glDeleteRenderbuffers(framebuffer->swap_chain_length,
                          framebuffer->depth_renderbuffers);

    info("free framebuffers");
    free(framebuffer->framebuffers);

    info("free depth renderbuffers");
    free(framebuffer->depth_renderbuffers);

    info("destroy color texture swap chain");
    vrapi_DestroyTextureSwapChain(framebuffer->color_texture_swap_chain);
}