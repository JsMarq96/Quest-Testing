//
// Created by Juan S. Marquerie on 13/06/2021.
//

#ifndef QUEST_DEMO_TEST_STAGE_H
#define QUEST_DEMO_TEST_STAGE_H

#include "../engine/scene.h"
#include "../engine/collision_types.h"
#include "../engine/input.h"

void test_scene_update(sScene                  *curr_scene,
                       const sControllerInput  *input,
                       const sCollisionManifold *manifolds,
                       const int                manifold_count,
                       const double             frame_delta);

#endif //QUEST_DEMO_TEST_STAGE_H
