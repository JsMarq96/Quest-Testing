//
// Created by js on 27/5/21.
//

#ifndef QUEST_DEMO_RAW_SHADERS_H
#define QUEST_DEMO_RAW_SHADERS_H

static const char borring_vertex_shader[] = "#version 300 es\n"
                                          "in vec3 a_v_pos;\n"
                                          "uniform mat4 u_model_mat;\n"
                                          "uniform mat4 u_view_mat;\n"
                                          "uniform mat4 u_proj_mat;\n"
                                          "void main()\n"
                                          "{\n"
                                          "	gl_Position = u_proj_mat * ( u_view_mat * ( (u_model_mat) * vec4( "
                                          "a_v_pos, 1.0 ) ) );\n"
                                          "}\n";

static const char borring_frag_shader[] = "#version 300 es\n"
                                        "\n"
                                        "out lowp vec4 out_color;\n"
                                        "void main()\n"
                                        "{\n"
                                        "	out_color = vec4(1.0, 0.0, 1.0, 1.0);\n"
                                        "}\n";

static const char basic_vertex_shader[] = "#version 300 es\n"
                                          "in vec3 a_v_pos;\n"
                                          "in vec2 a_uv_pos;\n"
                                          "uniform mat4 u_model_mat;\n"
                                          "uniform mat4 u_view_mat;\n"
                                          "uniform mat4 u_proj_mat;\n"
                                          "out vec2 uv_pos;\n"
                                          "void main()\n"
                                          "{\n"
                                          "	gl_Position = u_proj_mat * ( u_view_mat * ( (u_model_mat) * vec4( "
                                          "a_v_pos, 1.0 ) ) );\n"
                                          " uv_pos = a_uv_pos;\n"
                                          "}\n";

static const char basic_frag_shader[] = "#version 300 es\n"
                                        "\n"
                                        "in lowp vec2 uv_pos;\n"
                                        "out lowp vec4 out_color;\n"
                                        "uniform sampler2D u_colormap;\n"
                                        "void main()\n"
                                        "{\n"
                                        "	out_color = vec4(texture(u_colormap, uv_pos).xyz, 1.0);\n"
                                        "}\n";

static const char skybox_vertex_shader[] = "#version 300 es\n"
                                           "layout (location = 0) in vec3 a_v_pos;\n"
                                           "out vec3 text_coords;\n"
                                           "\n"
                                           "uniform mat4 u_proj_mat;\n"
                                           "uniform mat4 u_view_mat;\n"
                                           "\n"
                                           "void main()\n"
                                           "{\n"
                                           "    text_coords = a_v_pos;\n"
                                           "    vec4 pos = u_proj_mat * u_view_mat * vec4(a_v_pos * 20.0, 1.0);\n"
                                           "    gl_Position = pos.xyww;\n"
                                           "}  ";

static const char skybox_fragment_shader[] = "#version 300 es\n"
                                             "out vec4 frag_col;\n"
                                             "\n"
                                             "in vec3 text_coords;\n"
                                             "\n"
                                             "uniform samplerCube skybox;\n"
                                             "\n"
                                             "void main()\n"
                                             "{    \n"
                                             "    frag_col = texture(skybox, text_coords);\n"
                                             "}";

#endif //QUEST_DEMO_RAW_SHADERS_H
