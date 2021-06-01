//
// Created by Juan S. Marquerie on 01/06/2021.
//

#include "skybox_renderer.h"


void skybox_renderer_init(sSkyBoxRenderer  *renderer,
                          sTexture         *texture) {
    float raw_skybox_vertex[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
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
                 sizeof(raw_skybox_vertex),
                 raw_skybox_vertex,
                 GL_STATIC_DRAW);

    // Position parameter
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(float),
                          (void*) 0);

    glBindVertexArray(0);
}

void skybox_render(const sSkyBoxRenderer    *renderer,
                   const ovrTracking2       *tracking,
                   const unsigned int       eye) {
    ovrMatrix4f view_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye].ViewMatrix);
    ovrMatrix4f projection_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye].ProjectionMatrix);

    glBindVertexArray(renderer->VAO);
    glDepthFunc(GL_LEQUAL);

    renderer->shader.enable();

    renderer->shader.set_uniform_matrix4("u_view_mat", (sMat44*) &view_matrix);
    renderer->shader.set_uniform_matrix4("u_proj_mat", (sMat44*) &projection_matrix);

    glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->texture->texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    renderer->shader.disable();

    glDepthFunc(GL_LESS);

    glBindVertexArray(0);
}

void skybox_render_destroy(const sSkyBoxRenderer    *renderer) {
    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteBuffers(1, &renderer->VBO);

    // Delete shader
}