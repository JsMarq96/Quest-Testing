//
// Created by jsmar on 20/05/2021.
//
#ifndef _SHADER_H_
#define _SHADER_H_

#include <GLES3/gl3.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>


#include "math.h"

/**
 * Basic OpenGL Shader Class, for shader's IO boilerplate
 * Juan S. Marquerie
*/
struct sShader {
    unsigned int ID;

    sShader() {};
    sShader(const char* vertex_shader, const char* fragment_shader);

    void load_file_shaders(const char* v_shader_dir, const char* f_shader_dir);
    void load_shaders(const char* vertex_shader, const char* frag_shader_dir);

    void enable() const;
    void disable() const { glUseProgram(0); };

    // Setters for the shader's uniforms
    void set_uniform(const char* name, const float value) const;
    void set_uniform(const char* name, const int value) const;
    void set_uniform(const char* name, const bool value) const;
    void set_uniform_vector2(const char* name, const float value[2]) const;
    void set_uniform_vector3(const char* name, const float value[3]) const;
    void set_uniform_vector4(const char* name, const float value[4]) const;
    void set_uniform_matrix3(const char* name, const sMat33 *matrix) const;
    void set_uniform_matrix4(const char* name, const sMat44 *matrix) const;
    void set_uniform_matrix_array(const char* name, const sMat33 *mat_arr, const int len) const;
    void set_uniform_integet_array(const char* name, const int *array, const int len) const;
};

#endif