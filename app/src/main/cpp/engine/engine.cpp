//
// Created by jsmar on 11/06/2021.
//

#include "engine.h"

void ENGINE_add_scene(sEngineInstance   *engine,
                      const char        *scene_plan_dir,
                      void (*scene_update)(double frame_delta)) {
    sLinkedListScene *curr_link_instance;
    if (engine->start_scene_list != NULL) {
        engine->start_scene_list = (sLinkedListScene *) malloc(sizeof(sLinkedListScene));
        engine->curr_scene = engine->start_scene_list;
        curr_link_instance = engine->start_scene_list;
    } else {
        curr_link_instance = engine->start_scene_list;

        while (curr_link_instance->next != NULL) {
            curr_link_instance = curr_link_instance->next;
        }

        curr_link_instance->next = (sLinkedListScene *) malloc(sizeof(sLinkedListScene));
        curr_link_instance = curr_link_instance->next;
    }

    scene_init(&curr_link_instance->curr_scene);

    curr_link_instance->curr_scene.scene_update = scene_update;

    sScene *new_scene = &curr_link_instance->curr_scene;

    // Load the data for the Plan
    sAssMan *ass_man = &ass_man_instance;

    char *scene_plan = NULL;
    AM_fetch_asset(ass_man,
                   scene_plan_dir,
                   &scene_plan);

    FILE *plan_file;
    plan_file = fopen(scene_plan, "r");

    assert(plan_file != NULL && "Failed to open the plan file");

    int readed_chars;
    char *line_buffer = NULL;
    size_t len = 0;

    // Scene data
    // TODO: add Material not only textures
    while ((readed_chars = getline(&line_buffer, &len, plan_file)) != -1) {
        char buffer[50];
        int size;
        bool config;

        memset(buffer, '\0', 50 * sizeof(char));

        if (line_buffer[0] == 'N') { // name
            char name_buffer[10];
            sscanf(line_buffer, "NAME %s\n", &buffer);

        } else if (line_buffer[0] == 'S') { // set Skybox
            sscanf(line_buffer, "SKYBOX %s\n", &buffer);
            scene_set_skybox(new_scene, buffer);
        } else if (line_buffer[0] == 'M') {
            sscanf(line_buffer, "MESH %s %b\n", &buffer, &config);
            scene_resource_add_mesh(new_scene,
                                    buffer,
                                    config);
        } else if (line_buffer[0] == 'T') { // Add Material
            sscanf(line_buffer, "TEX %s\n", &buffer);
            scene_resource_add_material(new_scene,
                                        buffer,
                                        NULL,
                                        NULL,
                                        basic_vertex_shader,
                                        basic_frag_shader);
        } else if (line_buffer[0] == 'S' && line_buffer[6] == 'E') { // scene end
            break;
        }
    }

    // Data to fill
    sVector3 position{}, size{};
    sQuaternion4 rotation{};
    int mesh_index, material_index;
    char name[10];
    memset(name, '\0', 10 * sizeof(char));
    while ((readed_chars = getline(&line_buffer, &len, plan_file)) != -1) {
        char buffer[50];
        float x, y, z;

        memset(buffer, '\0', 50 * sizeof(char));

        if (line_buffer[0] == 'N') { // get Name
            sscanf(line_buffer, "NAME %s\n", &name);
        } else if (line_buffer[0] == 'P') { // Get position
            sscanf(line_buffer, "POSITION %f %f %f\n", &x, &y, &z);
            position.x = x;
            position.y = y;
            position.z = z;
        } else if (line_buffer[0] == 'R') { // Get Rotation
            sscanf(line_buffer, "ROTATION %f %f %f\n", &x, &y, &z);
            rotation.x = x;
            rotation.y = y;
            rotation.z = z;
        } else if (line_buffer[0] == 'M') { // Get mesh and material
            sscanf(line_buffer, "MESH-MATERIAL &d %d\n", &mesh_index, &material_index);
            mesh_index += STARTUP_MESH_COUNT;
            material_index += material_index;
        } else if (line_buffer[0] == 'O' && line_buffer[4] == 'E') { // Save as an object
            scene_add_object(new_scene,
                             name,
                             mesh_index,
                             material_index,
                             position);
        }

    }

    // TODO: Collider parsers

    fclose(plan_file);
}