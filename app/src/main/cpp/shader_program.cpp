//
// Created by jsmar on 20/05/2021.
//

#include "shader_program.h"

 GLuint
compile_shader(GLenum type, const char* string)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &string, NULL);
    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* log = (char*) malloc(length);
        glGetShaderInfoLog(shader, length, NULL, log);
        //error("can't compile shader: %s", log);
        exit(EXIT_FAILURE);
    }
    return shader;
}

 void
program_create(struct program* program)
{
    program->program = glCreateProgram();
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER);
    glAttachShader(program->program, vertex_shader);
    GLuint fragment_shader =
            compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    glAttachShader(program->program, fragment_shader);
    for (enum attrib attrib = ATTRIB_BEGIN; attrib != ATTRIB_END; attrib = (enum attrib) (int(attrib)+1) ) {
        glBindAttribLocation(program->program, attrib, ATTRIB_NAMES[attrib]);
    }

    glLinkProgram(program->program);
    GLint status = 0;
    glGetProgramiv(program->program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(program->program, GL_INFO_LOG_LENGTH, &length);
        char* log = (char*) malloc(length);
        glGetProgramInfoLog(program->program, length, NULL, log);
        //error("can't link program: %s", log);
        exit(EXIT_FAILURE);
    }
    for (enum uniform uniform = UNIFORM_BEGIN; uniform != UNIFORM_END; uniform = (enum uniform) (int(uniform)+1)) {
        program->uniform_locations[uniform] =
                glGetUniformLocation(program->program, UNIFORM_NAMES[uniform]);
    }
}

 void
program_destroy(struct program* program)
{
    glDeleteProgram(program->program);
}