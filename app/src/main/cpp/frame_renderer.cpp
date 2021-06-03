//
// Created by jsmar on 22/05/2021.s
//

#include "frame_renderer.h"

/// TODO: Crear destructor

void init_frame_renderer(sFrameRenderer *frame_rend,
                         const unsigned int width,
                         const unsigned int height) {
    for (int i = 0; i < VRAPI_FRAME_LAYER_EYE_MAX; ++i) {
        framebuffer_create(&frame_rend->framebuffers[i], width, height);
    }
}

void render_frame(sFrameRenderer *frame_render,
                  const sBatchMeshRenderer *renderer,
                  const sSkyBoxRenderer *skybox_renderer,
                  const ovrTracking2* tracking) {
    frame_render->frame_layer = vrapi_DefaultLayerProjection2();
    frame_render->frame_layer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;
    frame_render->frame_layer.HeadPose = tracking->HeadPose;

    // Config the layers and the opengl framebuffer
    for (int i = 0; i < VRAPI_FRAME_LAYER_EYE_MAX; ++i) {
        framebuffer *framebuffer = &frame_render->framebuffers[i];

        frame_render->frame_layer.Textures[i].ColorSwapChain = framebuffer->color_texture_swap_chain;
        frame_render->frame_layer.Textures[i].SwapChainIndex = framebuffer->swap_chain_index;
        frame_render->frame_layer.Textures[i].TexCoordsFromTanAngles = ovrMatrix4f_TanAngleMatrixFromProjection( &tracking->Eye[i].ProjectionMatrix);

        // Setup the framebuffers
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                          framebuffer->framebuffers[framebuffer->swap_chain_index]);

        //glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glViewport(0, 0, framebuffer->width, framebuffer->height);
        glScissor(0, 0, framebuffer->width, framebuffer->height);
        glClearColor(0.0, 0.0, 1.0, 0.5);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        info("Rendering Meshes");
        BMR_render(renderer, tracking, i);

        info("Rendering Skybox");
        skybox_render(skybox_renderer, tracking, i);

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glScissor(0, 0, 1, framebuffer->height);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(framebuffer->width - 1, 0, 1, framebuffer->height);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(0, 0, framebuffer->width, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(0, framebuffer->height - 1, framebuffer->width, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        static const GLenum ATTACHMENTS[] = { GL_DEPTH_ATTACHMENT };
        static const GLsizei NUM_ATTACHMENTS = sizeof(ATTACHMENTS) / sizeof(ATTACHMENTS[0]);
        glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER,
                                NUM_ATTACHMENTS,
                                ATTACHMENTS);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        framebuffer->swap_chain_index = (framebuffer->swap_chain_index + 1) % framebuffer->swap_chain_length;
    }
};