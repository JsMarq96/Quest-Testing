//
// Created by jsmar on 21/05/2021.
//

#ifndef QUEST_DEMO_MESH_RENDERER_H
#define QUEST_DEMO_MESH_RENDERER_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "../utils/common.h"
#include "framebuffer.h"
#include "shader.h"
#include "mesh.h"
#include "../utils/math.h"
#include "raw_shaders.h"

struct sMeshRenderer {
    unsigned int    VAO;
    unsigned int    VBO;
    unsigned int    EBO;

    const sMesh     *origin_mesh;
    int             indices_count = 0;

    sShader         shader;
};

void render_init(sMeshRenderer  *renderer,
                 const sMesh    *raw_mesh,
                 const bool      is_static);

void render_init_cube(sMeshRenderer *renderer,
                      const bool is_static);

void render_mesh(const sMeshRenderer *renderer,
                 const sMat44        *model_mat,
                 const ovrTracking2  *tracking,
                 const unsigned int   eye,
                 const bool           show_wireframe);

void render_destroy(sMeshRenderer  *renderer);

#endif //QUEST_DEMO_MESH_RENDERER_H
