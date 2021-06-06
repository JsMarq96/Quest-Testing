//
// Created by Juan S. Marquerie on 29/05/2021.
//

#ifndef QUEST_DEMO_SCENE_H
#define QUEST_DEMO_SCENE_H

#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "asset_manager.h"
#include "mesh.h"
#include "batch_mesh_renderer.h"
#include "skybox_renderer.h"
#include "math.h"

#define SCENE_OBJ_MAX 120
#define MAX_INSTANCE_SIZE 100

struct sScene {
    const char*     name;

    bool            initialized            [MAX_INSTANCE_SIZE]    = {false};
    bool            enabled                [MAX_INSTANCE_SIZE]    = {false};
    sVector3        position               [MAX_INSTANCE_SIZE]    = {sVector3{0.0f,0.0f, 0.0f}};
    sQuaternion4    rotation               [MAX_INSTANCE_SIZE]    = {sQuaternion4{0.0f,0.0f, 0.0f, 1.0f}};
    sRenderInstance render_instances       [MAX_INSTANCE_SIZE];

    sBatchMeshRenderer  mesh_renderer;
    sSkyBoxRenderer     skybox_renderer;
};

/// SCENE LIVECYCLE
void create_scene(sScene *new_scene);
void destroy_scene(sScene  *to_destroy);

int scene_resource_add_mesh(sScene       *scene,
                            const char*  texture,
                            const bool   is_static);

int scene_resource_add_material(sScene         *scene,
                                const char*    color_texture,
                                const char*    normal_texture,
                                const char*    specular_texture,
                                const char     *vertex_shader,
                                const char     *fragment_shader);

void scene_set_skybox(sScene      *scene,
                      const char  *skyboy_dir);

int scene_add_object(sScene          *scene,
                     const int       mesh_id,
                     const int       material_id,
                     const sVector3  position);

void scene_render(const sScene           *scene,
                  const ovrTracking2     *tracking,
                  const unsigned int     eye_index);

#endif //QUEST_DEMO_SCENE_H
