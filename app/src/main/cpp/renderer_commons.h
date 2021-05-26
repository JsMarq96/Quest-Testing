//
// Created by jsmar on 22/05/2021.
//

#ifndef QUEST_DEMO_RENDERER_COMMONS_H
#define QUEST_DEMO_RENDERER_COMMONS_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "framebuffer.h"

struct sToRenderFrame {
    ovrLayerProjection2    world_view_layer;
    framebuffer            framebuffers[VRAPI_FRAME_LAYER_EYE_MAX];
};

void init_frame_struct(sToRenderFrame *frame);

void prepare_frame(sToRenderFrame  *frame);

#endif //QUEST_DEMO_RENDERER_COMMONS_H
