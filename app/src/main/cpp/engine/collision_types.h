//
// Created by jsmar on 22/06/2021.
//

#ifndef QUEST_DEMO_COLLISION_TYPES_H
#define QUEST_DEMO_COLLISION_TYPES_H

#include "../utils/math.h"

struct sCollisionManifold {
    int collider1_index            = -1;
    int collider2_index            = -1;

    sVector3  collision_normal     = {};
    sVector3  contact_points    [4] = {};
    float     points_depth      [4] = {};
    int       contact_point_count   = 0;
};

#endif //QUEST_DEMO_COLLISION_TYPES_H
