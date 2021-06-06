//
// Created by Juan S. Marquerie on 01/06/2021.
//

#ifndef QUEST_DEMO_SKYBOX_RENDERER_H
#define QUEST_DEMO_SKYBOX_RENDERER_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "basic_framebuffer.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"
#include "math.h"
#include "material.h"

#include "raw_shaders.h"

struct sSkyBoxRenderer {
    unsigned int    VAO;
    unsigned int    VBO;

    sShader         shader;
    sTexture        texture;
    sMaterial       skybox_material;
};

void skybox_renderer_init(sSkyBoxRenderer  *renderer,
                          const char       *texture_dir);

void skybox_render(const sSkyBoxRenderer    *renderer,
                   const ovrTracking2       *tracking,
                   const unsigned int       eye);

void skybox_render_destroy(const sSkyBoxRenderer    *renderer);
#endif //QUEST_DEMO_SKYBOX_RENDERER_H
