//
// Created by jsmar on 21/05/2021.
//

#include "mesh_renderer.h"


void render_init(sMeshRenderer  *renderer,
                 const sMesh    *raw_mesh,
                 const bool      is_static) {
    renderer->origin_mesh = raw_mesh;
    renderer->indices_count = renderer->origin_mesh->indices_cout;

    glGenVertexArrays(1, &renderer->VAO);
    glGenBuffers(1, &renderer->VBO);
    glGenBuffers(1, &renderer->EBO);

    glBindVertexArray(renderer->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 raw_mesh->raw_vertex_size * sizeof(float),
                 raw_mesh->raw_vertex_list,
                 (is_static) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 raw_mesh->indices_cout * sizeof(unsigned int),
                 raw_mesh->faces_index,
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
}

void render_init_cube(sMeshRenderer *renderer,
                      const bool is_static) {
    float tmp_vertex[] = { -1.0f,  1.0f, -1.0f,
                           -1.0f, -1.0f, -1.0f,
                           1.0f, -1.0f, -1.0f,
                           1.0f, -1.0f, -1.0f,
                           1.0f,  1.0f, -1.0f,
                           -1.0f,  1.0f, -1.0f,

                           -1.0f, -1.0f,  1.0f,
                           -1.0f, -1.0f, -1.0f,
                           -1.0f,  1.0f, -1.0f,
                           -1.0f,  1.0f, -1.0f,
                           -1.0f,  1.0f,  1.0f,
                           -1.0f, -1.0f,  1.0f,

                           1.0f, -1.0f, -1.0f,
                           1.0f, -1.0f,  1.0f,
                           1.0f,  1.0f,  1.0f,
                           1.0f,  1.0f,  1.0f,
                           1.0f,  1.0f, -1.0f,
                           1.0f, -1.0f, -1.0f,

                           -1.0f, -1.0f,  1.0f,
                           -1.0f,  1.0f,  1.0f,
                           1.0f,  1.0f,  1.0f,
                           1.0f,  1.0f,  1.0f,
                           1.0f, -1.0f,  1.0f,
                           -1.0f, -1.0f,  1.0f,

                           -1.0f,  1.0f, -1.0f,
                           1.0f,  1.0f, -1.0f,
                           1.0f,  1.0f,  1.0f,
                           1.0f,  1.0f,  1.0f,
                           -1.0f,  1.0f,  1.0f,
                           -1.0f,  1.0f, -1.0f,

                           -1.0f, -1.0f, -1.0f,
                           -1.0f, -1.0f,  1.0f,
                           1.0f, -1.0f, -1.0f,
                           1.0f, -1.0f, -1.0f,
                           -1.0f, -1.0f,  1.0f,
                           1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &renderer->VAO);
    glGenBuffers(1, &renderer->VBO);

    glBindVertexArray(renderer->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(tmp_vertex),
                 tmp_vertex,
                 (is_static) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);


    // Position parameter
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(float),
                          (void*) 0);


    glBindVertexArray(0);

    renderer->indices_count = 36;

    renderer->shader.load_shaders(borring_vertex_shader, borring_frag_shader);
}



void render_mesh(const sMeshRenderer *renderer,
                 const sMat44        *model_mat,
                 const ovrTracking2  *tracking,
                 const unsigned int   eye,
                 const bool           show_wireframe) {
    ovrMatrix4f view_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye].ViewMatrix);
    ovrMatrix4f projection_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye].ProjectionMatrix);

    glBindVertexArray(renderer->VAO);

    renderer->shader.enable();

    renderer->shader.set_uniform_matrix4("u_model_mat", model_mat);
    renderer->shader.set_uniform_matrix4("u_view_mat", (sMat44*) &view_matrix);
    renderer->shader.set_uniform_matrix4("u_proj_mat", (sMat44*) &projection_matrix);

    glDrawArrays((show_wireframe) ? GL_LINES : GL_TRIANGLES,0, renderer->indices_count);

    info("Rendered mesh %i indexes", renderer->indices_count);

    renderer->shader.disable();


    glBindVertexArray(0);
}

void render_destroy(sMeshRenderer  *renderer) {
    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteBuffers(1, &renderer->VBO);
    glDeleteBuffers(1, &renderer->EBO);

    // Delete shader
}