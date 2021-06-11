//
// Created by jsmar on 02/06/2021.
//

#ifndef QUEST_DEMO_BATCH_MESH_RENDERER_H
#define QUEST_DEMO_BATCH_MESH_RENDERER_H

#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "material.h"
#include "mesh.h"

#define MAX_RESOURCE_SIZE 100

struct sOGLMeshIndexes {
    unsigned int    VAO           = 0;
    unsigned int    VBO           = 0;
    unsigned int    EBO           = 0;

    int             indices_count = 0;
};

struct sRenderInstance {
    int  material_index  = 0;
    int  mesh_index      = 0;
};

struct sBatchMeshRenderer {
    sMaterial       material_references     [MAX_RESOURCE_SIZE];
    sOGLMeshIndexes mesh_references         [MAX_RESOURCE_SIZE];

    int             last_inserted_material                        = 0;
    int             last_inserted_mesh                            = 0;
};

////  RESOURCE FUNCTIONS
int BMR_add_mesh(sBatchMeshRenderer *renderer,
                 const sMesh         *mesh,
                 const bool          is_static);

int BMR_add_mesh(sBatchMeshRenderer *renderer,
                 const char*         mesh_dir,
                 const bool          is_static);

int BMR_add_material(sBatchMeshRenderer *renderer,
                     const char*         color_texture,
                     const char*         normal_texture,
                     const char*         specular_texture,
                     const char          *vertex_shader,
                     const char          *fragment_shader);


//// INSTANCES FUNCTIONS
void BMR_render(const sBatchMeshRenderer   *renderer,
                const sMat44               *models,
                const sRenderInstance      *render_instances,
                const sVector3             *render_colors,
                const int                  obj_count,
                const ovrTracking2         *tracking,
                const unsigned int         eye_index);

//// LIFECYCLE FUNCTIONS
void BMR_destroy(sBatchMeshRenderer *renderer);

#endif //QUEST_DEMO_BATCH_MESH_RENDERER_H
