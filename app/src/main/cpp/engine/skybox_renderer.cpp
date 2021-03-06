//
// Created by Juan S. Marquerie on 01/06/2021.
//

#include "skybox_renderer.h"


void skybox_renderer_init(sSkyBoxRenderer  *renderer,
                          const char       *texture_dir) {
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

    renderer->skybox_material.shader.load_shaders(skybox_vertex_shader,
                                                  skybox_fragment_shader);

    material_add_cubemap_texture(&renderer->skybox_material,
                                 texture_dir);
}

void skybox_render(const sSkyBoxRenderer    *renderer,
                   const ovrTracking2       *tracking,
                   const unsigned int       eye) {
    ovrMatrix4f view_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye].ViewMatrix);
    ovrMatrix4f projection_matrix = ovrMatrix4f_Transpose(&tracking->Eye[eye].ProjectionMatrix);

    glBindVertexArray(renderer->VAO);
    glDepthFunc(GL_LEQUAL);

    renderer->shader.enable();

    material_enable(&(renderer->skybox_material));

    // TODO: This part is kinda yuck yuck bro...
    renderer->skybox_material.shader.set_uniform_matrix4("u_view_mat", (sMat44*) &view_matrix);
    renderer->skybox_material.shader.set_uniform_matrix4("u_proj_mat", (sMat44*) &projection_matrix);
    renderer->skybox_material.shader.set_uniform("skybox", 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    material_disable(&(renderer->skybox_material));

    glDepthFunc(GL_LESS);

    glBindVertexArray(0);
}

void skybox_render_destroy(const sSkyBoxRenderer    *renderer) {
    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteBuffers(1, &renderer->VBO);

    // Delete shader
}