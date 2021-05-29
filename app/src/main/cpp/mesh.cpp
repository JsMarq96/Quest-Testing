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
    result->faces_index = (unsigned int*) malloc(sizeof(float) * f_count * 3);
    result->indices_cout = f_count * 3;

    info("!!indices count %i vertex count : %i", result->indices_cout, v_count);

    // Rewind and then load the data
    rewind(mesh_file);

    int vertex_index = 0;
    int uv_index = 0;
    int faces_index = 0;
    char *pend;
    while((readed_chars = getline(&line_buffer, &len, mesh_file)) != -1) {
        if (line_buffer[0] == 'v' && line_buffer[1] == ' ') {
            float x, y, z;
            sscanf(line_buffer,"v %f %f %f\n",&x, &y, &z);
            result->vertex_list[vertex_index].x = x;
            result->vertex_list[vertex_index].y = y;
            result->vertex_list[vertex_index].z = z;
            vertex_index++;
        } else if (line_buffer[0] == 'v' && line_buffer[1] == 't') {
            //result->vertex_list[uv_index].u = 0.121;
            //result->vertex_list[uv_index].v = 0.212;
            //uv_index++;
        } else if (line_buffer[0] == 'f') {
            int index1, index2, index3, normal1, normal2, normal3, uv1, uv2, uv3;
            sscanf(line_buffer,
                   "f %i/%i/%i %i/%i/%i %i/%i/%i\n",
                   &index1,
                   &normal1,
                   &uv1,
                   &index2,
                   &normal2,
                   &uv2,
                   &index3,
                   &normal3,
                   &uv3);

            result->faces_index[faces_index++] = index1 - 1;
            result->faces_index[faces_index++] = index2 - 1;
            result->faces_index[faces_index++] = index3 - 1;
        }
    }

    fclose(mesh_file);
    result->vertex_count = vertex_index;
    result->raw_vertex_size = vertex_index * 5;
    result->uv_count = uv_index;
};


void
mesh_destroy(sMesh *to_dispose) {
    free(to_dispose->raw_vertex_list);
    free(to_dispose->faces_index);
};