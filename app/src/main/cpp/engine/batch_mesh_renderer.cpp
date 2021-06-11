//
// Created by jsmar on 02/06/2021.
//

#include "batch_mesh_renderer.h"
#include "../utils/common.h"

////  RESOURCE FUNCTIONS
int BMR_add_mesh(sBatchMeshRenderer *renderer,
                 const char*         mesh_dir,
                 const bool          is_static) {
    sMesh mesh;
    load_mesh(&mesh, mesh_dir);

    int mesh_index =  BMR_add_mesh(renderer, &mesh, is_static);

    mesh_destroy(&mesh);

    return mesh_index;
};

// TODO: add the surface normals
int BMR_add_mesh(sBatchMeshRenderer *renderer,
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
    renderer->mesh_references[renderer->last_inserted_mesh].indices_count = mesh->indices_cout;

    return renderer->last_inserted_mesh++;
}

int BMR_add_material(sBatchMeshRenderer *renderer,
                     const char*         color_texture,
                     const char*         normal_texture,
                     const char*         specular_texture,
                     const char          *vertex_shader,
                     const char          *fragment_shader) {

    sMaterial *material = &renderer->material_references[renderer->last_inserted_material];
    material_add_shader(material,
                        vertex_shader,
                        fragment_shader);

    if (color_texture != NULL) {
        material_add_texture(material, color_texture, (eTextureType) 0);
    }

    if (normal_texture != NULL) {
        material_add_texture(material, normal_texture, (eTextureType) 1);
    }

    if (specular_texture != NULL) {
        material_add_texture(material, specular_texture, (eTextureType) 2);
    }

    return renderer->last_inserted_material++;
};


//// INSTANCES FUNCTIONS

// TODO: batch togeter the rendering via instancing the same meshes
// TODO: Frustrum culling
void BMR_render(const sBatchMeshRenderer   *renderer,
                const sMat44               *models,
                const sRenderInstance      *render_instances,
                const sVector3             *render_colors,
                const int                  obj_count,
                const ovrTracking2         *tracking,
                const unsigned int         eye_index) {
    ovrMatrix4f view_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye_index].ViewMatrix);
    ovrMatrix4f projection_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye_index].ProjectionMatrix);

    for(int i = 0; i < obj_count; i++) {
        const sRenderInstance *rend_instance = &render_instances[i];
        const sOGLMeshIndexes *mesh_intance = &renderer->mesh_references[rend_instance->mesh_index];
        const sMaterial *material_instance = &renderer->material_references[rend_instance->material_index];

        glBindVertexArray(mesh_intance->VAO);

        material_enable(material_instance);

        sVector3 tmp = render_colors[i];

        // TODO: This is kinda yuck yuck bro
        material_instance->shader.set_uniform_vector3("u_color", render_colors[i].raw_values);
        material_instance->shader.set_uniform_matrix4("u_model_mat", &models[i]);
        material_instance->shader.set_uniform_matrix4("u_view_mat", (sMat44*) &view_matrix);
        material_instance->shader.set_uniform_matrix4("u_proj_mat", (sMat44*) &projection_matrix);
        material_instance->shader.set_uniform("u_colormap", 0);
        //material_instance->shader.set_uniform("u_normalmap", 1);
        //material_instance->shader.set_uniform("u_specularmap", 2);

        glDrawElements(GL_TRIANGLES, mesh_intance->indices_count, GL_UNSIGNED_INT, NULL);

        info("Rendered mesh %i  with %i indexes", i, mesh_intance->indices_count);

        material_disable(material_instance);

        glBindVertexArray(0);
    }
}

//// LIFECYCLE FUNCTIONS

void BMR_destroy(sBatchMeshRenderer *renderer) {
    for(int i = 0; i < MAX_RESOURCE_SIZE; i++) {
        glDeleteVertexArrays(1, &renderer->mesh_references[i].VAO);
        glDeleteBuffers(1, &renderer->mesh_references[i].VBO);
        glDeleteBuffers(1, &renderer->mesh_references[i].EBO);

        // TODO: Material cleanup
    }

    renderer->last_inserted_material = 0;
    renderer->last_inserted_mesh = 0;
}