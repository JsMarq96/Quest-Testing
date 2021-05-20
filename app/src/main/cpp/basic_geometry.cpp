//
// Created by jsmar on 20/05/2021.
//

#include "basic_geometry.h"

 void
geometry_create(struct geometry* geometry)
{
    glGenVertexArrays(1, &geometry->vertex_array);
    glBindVertexArray(geometry->vertex_array);
    glGenBuffers(1, &geometry->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    for (enum attrib attrib = ATTRIB_BEGIN; attrib != ATTRIB_END; attrib = (enum attrib) (int(attrib)+1)) {
        struct attrib_pointer attrib_pointer = ATTRIB_POINTERS[attrib];
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(attrib, attrib_pointer.size, attrib_pointer.type,
                              attrib_pointer.normalized, attrib_pointer.stride,
                              attrib_pointer.pointer);
    }


    glGenBuffers(1, &geometry->index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES,
                 GL_STATIC_DRAW);
    glBindVertexArray(0);
}

 void
geometry_destroy(struct geometry* geometry)
{
    glDeleteBuffers(1, &geometry->index_buffer);
    glDeleteBuffers(1, &geometry->vertex_buffer);
    glDeleteVertexArrays(1, &geometry->vertex_array);
}