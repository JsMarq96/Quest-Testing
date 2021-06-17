//
// Created by Juan S. Marquerie on 29/05/2021.
//

#ifndef QUEST_DEMO_SCENE_H
#define QUEST_DEMO_SCENE_H

#include <VrApi.h>
#include <VrApi_Helpers.h>
#include <string.h>

#include "../utils/math.h"
#include "../utils/kv_storage.h"
#include "../utils/asset_manager.h"

#include "mesh.h"
#include "batch_mesh_renderer.h"
#include "skybox_renderer.h"
#include "collision_controller.h"
#include "input.h"

#define MAX_INSTANCE_SIZE 100

enum eObjectAttributes : unsigned char {
    SOLID     = 0b000001,
    KINEMATIC = 0b000010
};

struct sScene {
    const char*     name;

    /// SCENE DATA
    bool            initialized            [MAX_INSTANCE_SIZE]    = { false };
    bool            enabled                [MAX_INSTANCE_SIZE]    = { false };
    sVector3        position               [MAX_INSTANCE_SIZE]    = { sVector3{0.0f,0.0f, 0.0f} };
    sQuaternion4    rotation               [MAX_INSTANCE_SIZE]    = { sQuaternion4{0.0f,0.0f, 0.0f, 1.0f} };
    sRenderInstance render_instances       [MAX_INSTANCE_SIZE];
    sVector3        obj_highlight_color    [MAX_INSTANCE_SIZE]    = { sVector3{1.f, 1.f, 1.f} };

    /// FOR ACCESSING THE DATA FROM THE UPDATE FUNCTIONS
    sKVStorage      obj_index_storage;
    sKVStorage      mesh_index_storage;
    sKVStorage      material_index_storage;

    /// GAMELOOP COMPONENTS
    sColliderController  collision_controller;

    /// RENDER COMPONENTS
    sBatchMeshRenderer  mesh_renderer;
    sSkyBoxRenderer     skybox_renderer;

    /// GAME LOOP FUNCTIONS
    void (*scene_update)(sScene *curr_scene, const sControllerInput  *input, const double frame_delta) = NULL;
};

/// SCENE LIVECYCLE
void scene_init(sScene        *new_scene);

void scene_destroy(sScene  *to_destroy);

void scene_update(sScene *scene,
                  const sControllerInput  *input,
                  const double elapsed_time);

void scene_render(const sScene           *scene,
                  const ovrTracking2     *tracking,
                  const unsigned int     eye_index);



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
                     const char      *obj_tag,
                     const int       mesh_id,
                     const int       material_id,
                     const sVector3  position);

int scene_add_collider(sScene          *scene,
                       const sVector3  position,
                       const sVector3 dimensions,
                       const sQuaternion4 rotation);

void scene_attach_collider_to_object(sScene           *scene,
                                     const int        colder_index,
                                     const int        obj_index,
                                     const sVector3   relative_position);

inline int scene_fetch_object_id(const sScene* scene,
                                 const char* tag) {
    return KVS_get_int((sKVStorage*) &scene->obj_index_storage,
                       tag,
                       strlen(tag));
}

#endif //QUEST_DEMO_SCENE_H
