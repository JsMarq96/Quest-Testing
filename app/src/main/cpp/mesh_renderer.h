//
// Created by jsmar on 21/05/2021.
//

#ifndef QUEST_DEMO_MESH_RENDERER_H
#define QUEST_DEMO_MESH_RENDERER_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "framebuffer.h"
#include "shader.h"
#include "mesh.h"
#include "math.h"

struct sMeshRenderer {
    unsigned int    VAO;
    unsigned int    VBO;
    unsigned int    EBO;

    sShader         shader;

    const sMesh           *origin_mesh;
};

void render_init(sMeshRenderer  *renderer,
                 const sMesh    *raw_mesh,
                 const bool      is_static);

void render_mesh(framebuffer         *framebuffer,
                 const sMeshRenderer *renderer,
                 const sMat44        *model_mat,
                 const ovrTracking2  *tracking);

void render_destroy();
#endif //QUEST_DEMO_MESH_RENDERER_H
