//
// Created by jsmar on 11/06/2021.
//

#ifndef QUEST_DEMO_ENGINE_H
#define QUEST_DEMO_ENGINE_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "raw_shaders.h"
#include "scene.h"
#include "../utils/json.h"

#define STARTUP_MESH_COUNT 2
#define STARTUP_MATERIAL_COUNT 2

struct sLinkedListScene {
    sLinkedListScene  *next   = NULL;

    sScene curr_scene;
};

struct sEngineInstance {
    sLinkedListScene *start_scene_list  = NULL;

    sLinkedListScene  *curr_scene = NULL;
};

void ENGINE_start();

void ENGINE_add_scene(sEngineInstance   *engine,
                      const char        *engine_dir);

void ENGINE_delete();

#endif //QUEST_DEMO_ENGINE_H
