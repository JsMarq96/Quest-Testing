//
// Created by js on 27/5/21.
//

#ifndef QUEST_DEMO_RAW_SHADERS_H
#define QUEST_DEMO_RAW_SHADERS_H

static const char basic_vertex_shader[] = "#version 300 es\n"
                                          "in vec3 a_v_pos;\n"
                                          "in vec2 a_uv_pos;\n"
                                          "uniform mat4 u_model_mat;\n"
                                          "uniform mat4 u_view_mat;\n"
                                          "uniform mat4 u_proj_mat;\n"
                                          "\n"
                                          "out vec3 vert_color;\n"
                                          "void main()\n"
                                          "{\n"
                                          "	gl_Position = u_proj_mat * ( u_view_mat * ( (u_model_mat) * vec4( "
                                          "a_v_pos * 0.1, 1.0 ) ) );\n"
                                          "	vert_color = vec3(1.0, 0.0, 0.0);\n"
                                          "}\n";

static const char basic_frag_shader[] = "#version 300 es\n"
                                        "\n"
                                        "in lowp vec3 vert_color;\n"
                                        "out lowp vec4 out_color;\n"
                                        "void main()\n"
                                        "{\n"
                                        "	out_color = vec4(vec3(1.0, 1.0, 1.0), 1.0);\n"
                                        "}\n";

#endif //QUEST_DEMO_RAW_SHADERS_H
