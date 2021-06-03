//
// Created by jsmar on 02/06/2021.
//

#ifndef QUEST_DEMO_BATCH_MESH_RENDERER_H
#define QUEST_DEMO_BATCH_MESH_RENDERER_H

#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "material.h"
#include "mesh.h"

#define MAX_INSTANCE_SIZE 100
#define MAX_RESOURCE_SIZE 50

struct sOGLMeshIndexes {
    unsigned int    VAO           = 0;
    unsigned int    VBO           = 0;
    unsigned int    EBO           = 0;

    int             indices_count = 0;
};

struct sBatchMeshRenderer {
    bool            enabled                [MAX_INSTANCE_SIZE]    = {false};
    sMat44          models                 [MAX_INSTANCE_SIZE];
    unsigned int    mesh_instance_ids      [MAX_INSTANCE_SIZE]    = { 0 };
    unsigned int    material_instance_ids  [MAX_INSTANCE_SIZE]    = { 0 };

    sMaterial       material_references     [MAX_RESOURCE_SIZE];
    sOGLMeshIndexes mesh_references         [MAX_RESOURCE_SIZE];
    int             last_inserted_material                        = 0;
    int             last_inserted_mesh                            = 0;
};

////  RESOURCE FUNCTIONS
int BMR_add_mesh(sBatchMeshRenderer *renderer,
                 const sMesh         *mesh,
                 const bool          is_static);

int BMR_add_material(sBatchMeshRenderer *renderer,
                     sTexture            *texture,
                     const char          *vertex_shader,
                     const char          *fragment_shader);


//// INSTANCES FUNCTIONS
int BMR_add_instance(sBatchMeshRenderer  *renderer,
                     const int             mesh_id,
                     const int             material_id,
                     const sVector3        position);

void BMR_render(const sBatchMeshRenderer  *renderer,
                const ovrTracking2   *tracking,
                const unsigned int   eye_index);

//// LIFECYCLE FUNCTIONS
void BMR_destroy(sBatchMeshRenderer *renderer);

#endif //QUEST_DEMO_BATCH_MESH_RENDERER_H
