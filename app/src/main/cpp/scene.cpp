//
// Created by Juan S. Marquerie on 29/05/2021.
//

#include "scene.h"


void create_scene(sScene *new_scene) {
    // ??
}

void destroy_scene(sScene  *to_destroy){
    for (int i = 0; i < SCENE_OBJ_MAX; i++) {
        if (!to_destroy->enabled[i]) {
            continue;
        }
        render_destroy(&to_destroy->renderers[i]);
    }
}


int add_object_to_scene(sScene  *scene,
                        const sMesh *mesh,
                        const sMat44  *model) {

}

void delete_object_from_scene(sScene *scene,
                              const int object_index) {

}