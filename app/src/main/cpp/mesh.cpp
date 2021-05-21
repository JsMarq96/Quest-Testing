//
// Created by jsmar on 21/05/2021.
//

#include "mesh.h"

void
load_mesh(sMesh        *result,
          const char*   mesh_file_dir) {
    FILE *mesh_file;
    int mesh_file_size;
    char *raw_mesh_file;

    mesh_file = fopen(mesh_file_dir, "r");

    assert(mesh_file != NULL && "Failed to open the mesh");

    // First gen the vertex/ faces count
    int v_count = 0;
    int f_count = 0;

    int readed_chars;
    char *line_buffer = NULL;
    size_t len = 0;
    while((readed_chars = getline(&line_buffer, &len, mesh_file)) != -1) {
        if (line_buffer[0] == 'v' && line_buffer[1] == ' ') {
            v_count++;
        } else if (line_buffer[0] == 'f') {
            f_count++;
        }
    }

    // Allocate the memmory
    result->vertex_list = (sGlVertex*) malloc(sizeof(sGlVertex) * v_count);
    result->vertex_count = v_count;
    result->faces_index = (unsigned int*) malloc(sizeof(float) * f_count * 9);
    result->indices_cout = f_count * 9;

    // Rewind and then load the data
    rewind(mesh_file);

    int vertex_index = 0;
    int uv_index = 0;
    int faces_index = 0;
    char *pend;
    while((readed_chars = getline(&line_buffer, &len, mesh_file)) != -1) {
        if (line_buffer[0] == 'v' && line_buffer[1] == ' ') {
            result->vertex_list[vertex_index].x = strtof(line_buffer, &pend);
            result->vertex_list[vertex_index].y = strtof(pend, NULL);
            result->vertex_list[vertex_index].z = strtof(pend, NULL);
            vertex_index++;
        } else if (line_buffer[0] == 'v' && line_buffer[1] == 't') {
            result->vertex_list[uv_index].x = strtof(line_buffer, &pend);
            result->vertex_list[uv_index].y = strtof(pend, NULL);
            uv_index++;
        } else if (line_buffer[0] == 'f') {
            // TODO: HORRIBLE TO REWORK BUT WANT TO GET IT DONE IM SORRY MOM
            for (int i = 0; i < 3; i++) {
                result->faces_index[faces_index++] = (int) strtof(line_buffer, &pend);
                result->faces_index[faces_index++] = (int) strtof(pend, NULL);
                result->faces_index[faces_index++] = (int) strtof(pend, NULL);
            }
        }
    }

    fclose(mesh_file);
};


void
mesh_destroy(sMesh *to_dispose) {
    free(to_dispose->raw_vertex_list);
    free(to_dispose->faces_index);
};