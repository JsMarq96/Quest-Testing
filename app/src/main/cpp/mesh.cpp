//
// Created by jsmar on 21/05/2021.
//

#include "mesh.h"
#include "common.h"

// NOTE: this only works for TRIANGLE ONLY meshes
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

    info("v count: %i", v_count);
    // Allocate the memmory
    result->vertex_list = (sGlVertex*) malloc(sizeof(sGlVertex) * v_count);
    result->vertex_count = v_count;
    result->raw_vertex_size = v_count * 5;
    result->faces_index = (unsigned int*) malloc(sizeof(float) * f_count * 9);
    result->indices_cout = f_count * 9;

    info("!!indices count %i vertex count : %i", result->indices_cout, v_count);

    // Rewind and then load the data
    rewind(mesh_file);

    int vertex_index = 0;
    int uv_index = 0;
    int faces_index = 0;
    char *pend;
    while((readed_chars = getline(&line_buffer, &len, mesh_file)) != -1) {
        if (line_buffer[0] == 'v' && line_buffer[1] == ' ') {
            result->vertex_list[vertex_index].x = strtof(&line_buffer[1], &pend);
            result->vertex_list[vertex_index].y = strtof(pend, NULL);
            result->vertex_list[vertex_index].z = strtof(pend, NULL);
            //info("r%i : %f, ")
            vertex_index++;
        } else if (line_buffer[0] == 'v' && line_buffer[1] == 't') {
            result->vertex_list[uv_index].u = strtof(&line_buffer[2], &pend);
            result->vertex_list[uv_index].v = strtof(pend, NULL);
            uv_index++;
        } else if (line_buffer[0] == 'f') {
            // TODO: HORRIBLE TO REWORK BUT WANT TO GET IT DONE IM SORRY MOM
            // Replace all the ocurrences of the / char in the faces description
            for (char *current_pos = strchr(line_buffer, '/');
                (current_pos = strchr(line_buffer, '/')) != NULL;
                *current_pos = ' ');

            for (int i = 0; i < 3; i++) {
                // -1 since the obj indexes start at 1
                result->faces_index[faces_index++] = (int) strtof(&line_buffer[1], &pend) - 1;
                result->faces_index[faces_index++] = (int) strtof(pend, NULL) - 1;
                result->faces_index[faces_index++] = (int) strtof(pend, NULL) - 1;
            }
        }
    }

    fclose(mesh_file);
    result->vertex_count = vertex_index;
    result->uv_count = uv_index;
    info("v size %i", vertex_index);
};


void
mesh_destroy(sMesh *to_dispose) {
    free(to_dispose->raw_vertex_list);
    free(to_dispose->faces_index);
};