//
// Created by jsmar on 22/05/2021.
//

#ifndef QUEST_DEMO_FRAME_RENDERER_H
#define QUEST_DEMO_FRAME_RENDERER_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "mesh_renderer.h"
#include "framebuffer.h"

struct sFrameRenderer {
    framebuffer            framebuffers[VRAPI_FRAME_LAYER_EYE_MAX];
    ovrLayerProjection2    frame_layer;
};


void render_frame(sFrameRenderer *frame_render,
                  const sMeshRenderer *renderers,
                  const unsigned int mesh_count,
                  const ovrTracking2* tracking);

#endif //QUEST_DEMO_FRAME_RENDERER_H
