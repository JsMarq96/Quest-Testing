//
// Created by jsmar on 21/05/2021.
//

#include "mesh_renderer.h"


void render_init(sMeshRenderer  *renderer,
                 const sMesh    *raw_mesh,
                 const bool      is_static) {
    renderer->origin_mesh = raw_mesh;

    glGenVertexArrays(1, &renderer->VAO);
    glGenBuffers(1, &renderer->VBO);
    glGenBuffers(1, &renderer->EBO);

    glBindVertexArray(renderer->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 raw_mesh->raw_vertex_size,
                 raw_mesh->raw_vertex_list,
                 (is_static) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 raw_mesh->indices_cout,
                 raw_mesh->faces_index,
                 (is_static) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    // Position parameter
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(float),
                          (void*) 0);

    // UV coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          2 * sizeof(float),
                          (void*) (sizeof(float) * 3));

    glBindVertexArray(0);
}

void render_mesh(framebuffer         *framebuffer,
                 const sMeshRenderer *renderer,
                 const sMat44        *model_mat,
                 const ovrTracking2  *tracking) {
    for(int i = 0; i < VRAPI_FRAME_LAYER_EYE_MAX; i++) {
        ovrMatrix4f view_matrix = ovrMatrix4f_Transpose(&tracking->Eye[i].ViewMatrix);
        ovrMatrix4f projection_matrix = ovrMatrix4f_Transpose(&tracking->Eye[i].ProjectionMatrix);

        renderer->shader.enable();

        renderer->shader.set_uniform_matrix4("u_model_mat", model_mat);
        renderer->shader.set_uniform_matrix4("u_view_mat", (sMat44*) &view_matrix);
        renderer->shader.set_uniform_matrix4("u_proj_mat", (sMat44*) &projection_matrix);

        glBindVertexArray(renderer->VAO);
        glDrawElements(GL_TRIANGLES, renderer->origin_mesh->indices_cout, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);

        info("Rendered %i indexes with %i vertex", renderer->origin_mesh->indices_cout, renderer->origin_mesh->vertex_count);

        renderer->shader.disable();
    }
}

void render_destroy() {

}