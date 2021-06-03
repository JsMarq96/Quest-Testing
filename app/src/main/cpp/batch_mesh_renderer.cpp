//
// Created by jsmar on 02/06/2021.
//

#include "batch_mesh_renderer.h"
#include "common.h"

////  RESOURCE FUNCTIONS

// TODO: add the surface normals
int BMR_add_mesh(sBatchFrameRenderer *renderer,
                 const sMesh         *mesh,
                 const bool          is_static) {
    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh->raw_vertex_size * sizeof(float),
                 mesh->raw_vertex_list,
                 (is_static) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh->indices_cout * sizeof(unsigned int),
                 mesh->faces_index,
                 (is_static) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    // Position parameter
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void*) 0);

    // UV coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void*) (sizeof(float) * 3));

    glBindVertexArray(0);

    renderer->mesh_references[renderer->last_inserted_mesh].VAO = VAO;
    renderer->mesh_references[renderer->last_inserted_mesh].VBO = VBO;
    renderer->mesh_references[renderer->last_inserted_mesh].EBO = EBO;

    return renderer->last_inserted_mesh++;
}

int BMR_add_material(sBatchFrameRenderer *renderer,
                     sTexture            *texture[3],
                     const char          *vertex_shader,
                     const char          *fragment_shader) {
    sMaterial *material = &renderer->material_references[renderer->last_inserted_material];
    material_add_shader(material,
                        vertex_shader,
                        fragment_shader);

    for (int i = 0; i < 3; i++) {
        if (texture[i] == NULL) {
            continue;
        }

        material_add_texture(material, texture[i], (eTextureType) i);
    }

    return renderer->last_inserted_material++;
}


//// INSTANCES FUNCTIONS
int BMR_add_instance(sBatchFrameRenderer  *renderer,
                     const int             mesh_id,
                     const int             material_id,
                     const sVector3        position) {
    int index;
    for (index = 0; index < MAX_INSTANCE_SIZE; index++) {
        if (!renderer->enabled[index]) {
            break;
        }
    }

    renderer->enabled[index] = true;
    renderer->models[index].set_position(position);
    renderer->mesh_instance_ids[index] = mesh_id;
    renderer->material_instance_ids[index] = material_id;

    return index;
}

// TODO: batch togeter the rendering via instancing the same meshes
// TODO: Frustrum culling
void BMR_render(sBatchFrameRenderer  *renderer,
                const ovrTracking2   *tracking,
                const unsigned int   eye_index) {
    ovrMatrix4f view_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye_index].ViewMatrix);
    ovrMatrix4f projection_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye_index].ProjectionMatrix);

    for(int i = 0; i < MAX_INSTANCE_SIZE; i++) {
        if (!renderer->enabled[i]) {
            continue;
        }

        sOGLMeshIndexes *mesh_intance = &renderer->mesh_references[renderer->mesh_instance_ids[i]];
        sMaterial *material_instance = &renderer->material_references[renderer->material_instance_ids[i]];

        glBindVertexArray(mesh_intance->VAO);

        material_enable(material_instance);

        material_instance->shader.set_uniform_matrix4("u_model_mat", &renderer->models[i]);
        material_instance->shader.set_uniform_matrix4("u_view_mat", (sMat44*) &view_matrix);
        material_instance->shader.set_uniform_matrix4("u_proj_mat", (sMat44*) &projection_matrix);

        glDrawElements(GL_TRIANGLES, mesh_intance->indices_count, GL_UNSIGNED_INT, NULL);

        info("Rendered mesh %i  with %i indexes", i, mesh_intance->indices_count);

        material_disable(material_instance);

        glBindVertexArray(0);
    }
}

//// LIFECYCLE FUNCTIONS
void BMR_destroy(sBatchFrameRenderer *renderer) {
    for(int i = 0; i < MAX_RESOURCE_SIZE; i++) {
        if (!renderer->enabled[i]) {
            continue;
        }
        glDeleteVertexArrays(1, &renderer->mesh_references[i].VAO);
        glDeleteBuffers(1, &renderer->mesh_references[i].VBO);
        glDeleteBuffers(1, &renderer->mesh_references[i].EBO);

        // TODO: Material cleanup
    }

    renderer->last_inserted_material = 0;
    renderer->last_inserted_mesh = 0;
}