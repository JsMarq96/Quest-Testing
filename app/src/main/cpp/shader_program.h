//
// Created by jsmar on 20/05/2021.
//

#ifndef QUEST_DEMO_SHADER_PROGRAM_H
#define QUEST_DEMO_SHADER_PROGRAM_H

#include <GLES3/gl3.h>
#include <VrApi.h>
#include <stdlib.h>

#include "common.h"

enum attrib
{
    ATTRIB_BEGIN = 0,
    ATTRIB_POSITION = ATTRIB_BEGIN,
    ATTRIB_COLOR = 1,
    ATTRIB_END = 2,
};

enum uniform
{
    UNIFORM_BEGIN = 0,
    UNIFORM_MODEL_MATRIX = UNIFORM_BEGIN,
    UNIFORM_VIEW_MATRIX = 1,
    UNIFORM_PROJECTION_MATRIX = 2,
    UNIFORM_END = 3,
};

struct program
{
    GLuint program;
    GLint uniform_locations[UNIFORM_END];
};

static const char* ATTRIB_NAMES[ATTRIB_END] = {
        "aPosition", "aColor",
};

static const char* UNIFORM_NAMES[UNIFORM_END] = {
        "uModelMatrix", "uViewMatrix", "uProjectionMatrix",
};

static const char VERTEX_SHADER[] =
        "#version 300 es\n"
        "\n"
        "in vec3 aPosition;\n"
        "in vec3 aColor;\n"
        "uniform mat4 uModelMatrix;\n"
        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "\n"
        "out vec3 vColor;\n"
        "void main()\n"
        "{\n"
        "	gl_Position = uProjectionMatrix * ( uViewMatrix * ( uModelMatrix * vec4( "
        "aPosition * 0.1, 1.0 ) ) );\n"
        "	vColor = aColor;\n"
        "}\n";

static const char FRAGMENT_SHADER[] = "#version 300 es\n"
                                      "\n"
                                      "in lowp vec3 vColor;\n"
                                      "out lowp vec4 outColor;\n"
                                      "void main()\n"
                                      "{\n"
                                      "	outColor = vec4(vColor, 1.0);\n"
                                      "}\n";

GLuint
compile_shader(GLenum type, const char* string);

void
program_create(struct program* program);

void
program_destroy(struct program* program);

#endif //QUEST_DEMO_SHADER_PROGRAM_H
