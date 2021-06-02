//
// Created by Juan S. Marquerie on 29/05/2021.
//

#ifndef QUEST_DEMO_SCENE_H
#define QUEST_DEMO_SCENE_H

#include <VrApi.h>
#include <VrApi_Helpers.h>

#include "mesh.h"
#include "mesh_renderer.h"
#include "math.h"

#define SCENE_OBJ_MAX 120

struct sScene {
    bool            enabled     [SCENE_OBJ_MAX] = {false};
    sMeshRenderer   renderers   [SCENE_OBJ_MAX];
    sMat44          models      [SCENE_OBJ_MAX];
};

/// Scene Live Cicle
void create_scene(sScene *new_scene);
void destroy_scene(sScene  *to_destroy);


int add_object_to_scene(sScene  *scene,
                        const sMesh *mesh,
                        const sMat44  *model);

void delete_object_from_scene(sScene *scene,
                              const int object_index);

inline void move_object_in_scene(sScene          *scene,
                                 const int       object_index,
                                 const sVector3  position) {
    scene->models[object_index].set_position(position);
}

#endif //QUEST_DEMO_SCENE_H
