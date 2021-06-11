//
// Created by jsmar on 05/06/2021.
//

#include "framebuffer.h"


// TODO: add security checks
// TODO: does the swapchain size needs to be dynamic..?
void framebuffers_init(sFramebuffers  *to_init,
                       const int      width,
                       const int      height) {
    to_init->ovr_color_swapchain = vrapi_CreateTextureSwapChain3(VRAPI_TEXTURE_TYPE_2D,
                                                                 GL_RGBA8,
                                                                 width,
                                                                 height,
                                                                 1,
                                                                 3);

    // Allocate buffers
    glGenFramebuffers(SWAPCHAIN_LEN,
                       to_init->framebuffers);
    glGenRenderbuffers(SWAPCHAIN_LEN,
                       to_init->depth_renderbuffers);

    for (int i = 0; i < SWAPCHAIN_LEN; i++) {
        unsigned int color_text = vrapi_GetTextureSwapChainHandle(to_init->ovr_color_swapchain, i);
        // Create color texture
        glBindTexture(GL_TEXTURE_2D, color_text);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create depth renderbuffer
        glBindRenderbuffer(GL_RENDERBUFFER, to_init->depth_renderbuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Create framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to_init->framebuffers[i]);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, color_text, 0);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,to_init->depth_renderbuffers[i]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    to_init->width = width;
    to_init->height = height;
    to_init->swapchain_index = 0;
}

void framebuffers_destroy(sFramebuffers *to_destroy) {
    glDeleteFramebuffers(SWAPCHAIN_LEN, to_destroy->framebuffers);
    glDeleteRenderbuffers(SWAPCHAIN_LEN, to_destroy->depth_renderbuffers);

    vrapi_DestroyTextureSwapChain(to_destroy->ovr_color_swapchain);
}