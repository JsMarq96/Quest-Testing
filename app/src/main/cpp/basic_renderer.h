//
// Created by jsmar on 20/05/2021.
//

#ifndef QUEST_DEMO_BASIC_RENDERER_H
#define QUEST_DEMO_BASIC_RENDERER_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "common.h"
#include "basic_framebuffer.h"
#include "basic_renderer.h"
#include "basic_geometry.h"
#include "mesh_renderer.h"

struct renderer
{
    struct basic_framebuffer framebuffers[VRAPI_FRAME_LAYER_EYE_MAX];
    struct program program;
    struct geometry geometry;
};

 void
renderer_create(struct renderer* renderer, GLsizei width, GLsizei height);

 void
renderer_destroy(struct renderer* renderer);

 ovrLayerProjection2
renderer_render_frame(struct renderer* renderer, ovrTracking2* tracking);
#endif //QUEST_DEMO_BASIC_RENDERER_H
