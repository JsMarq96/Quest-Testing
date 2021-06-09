//
// Created by jsmar on 07/06/2021.
//

#ifndef QUEST_DEMO_COLLIDER_CONTROLLER_H
#define QUEST_DEMO_COLLIDER_CONTROLLER_H

#include <VrApi.h>

#include <math.h>
#include "math.h"
#include "mesh_renderer.h"

#define MAX_SCENE_COLLIDERS 100
#define TAG_SIZE 4

/**
 * AABB collider utility.
 * Collision resolution will come later
 * A collider can be entangles to a scene object, and move and rotate with it
 * */

enum eColliderType : int {
    AABB_COLLIDER = 0,
    SPHERE_COLLIDER,
    BOX_COLLIDER
};

struct sCollision {
    int collider1_index           = -1;
    int collider2_index           = -1;

    int collider1_entangled_object = -1;
    int collider2_entangled_object = -1;

    char collider1_tag[TAG_SIZE]   = "";
    char collider2_tag[TAG_SIZE]   = "";
};

#define DEBUG TRUE

struct sColliderController {
    bool      enabled_colliders            [MAX_SCENE_COLLIDERS] = { false };
    int       collider_object_entanglement [MAX_SCENE_COLLIDERS] = { -1 };

    eColliderType  collider_type           [MAX_SCENE_COLLIDERS] = { AABB_COLLIDER };
    sVector3       collider_origin_points  [MAX_SCENE_COLLIDERS] = {sVector3{0.0f, 0.f, 0.0f} };
    sVector3       box_collider_sizes      [MAX_SCENE_COLLIDERS] = {sVector3{0.0f, 0.f, 0.0f} };
    sQuaternion4   box_collider_rotations  [MAX_SCENE_COLLIDERS] = { sQuaternion4{0.0f, 0.0f, 0.0f, 0.0f} };
    float          sphere_collider_radius  [MAX_SCENE_COLLIDERS] = { 0.0f };

    char           collider_tag[TAG_SIZE]  [MAX_SCENE_COLLIDERS] = {""};

    sVector3  entangled_position_delta     [MAX_SCENE_COLLIDERS] = {sVector3{0.0f, 0.f, 0.0f}};

    // TODO: Add labels for the collisions
    // TODO: Add triggers for collisions

    sMeshRenderer debug_renderer;
    // TODO add mesh renderers for sphere colliders
};

//// LIFECYCLE FUNCTIONS
void CC_init(sColliderController *col_contr);

void CC_update(sColliderController    *col_contr,
               const sVector3         *obj_positions,
               const sQuaternion4     *obj_rotations,
               sCollision             *result_collisions,
               int                    *collision_count);

void CC_render(const sColliderController *col_contr,
               const ovrTracking2         *tracking,
               const unsigned int         eye_index);


//// COLLIDER MANAGING FUNCTIONS

int CC_add_AABB_collider(sColliderController *controller,
                         const sVector3 position,
                         const float width,
                         const float heigth,
                         const float depth);

int CC_add_AABB_collider(sColliderController *controller,
                         const sVector3      position,
                         const float         width,
                         const float         heigth,
                         const float         depth);

int CC_add_Box_collider(sColliderController *controller,
                        const sVector3       position,
                        const sQuaternion4   rotation,
                        const float          width,
                        const float          heigth,
                        const float          depth);

//// COLLISION TESTING FUNCTIONS
// TODO: BOX COLLIDER FUNCTIONS
inline bool test_AABB_AABB_collision(const sVector3  center1,
                                     const sVector3  size1,
                                     const sVector3  center2,
                                     const sVector3  size2) {
    return  (center1.x <= (center2.x + size2.x) && (center1.x + size1.x) >= center2.x) &&
            (center1.y <= (center2.y + size2.y) && (center1.y + size1.y) >= center2.y) &&
            (center1.z <= (center2.z + size2.z) && (center1.z + size1.z) >= center2.z);
}

inline bool test_sphere_point_collision(const sVector3  center1,
                                        const float     radius1,
                                        const sVector3  point) {
    float center_distance = sqrt(((center1.x - point.x) * (center1.x - point.x)) +
                                 ((center1.y - point.y) * (center1.y - point.y)) +
                                 ((center1.z - point.z) * (center1.z - point.z)));
    return center_distance < radius1;
}

inline bool test_AABB_sphere_collision(const sVector3  center1,
                                       const sVector3  size1,
                                       const sVector3  center2,
                                       const float     radius) {
    float closest_x = MAX(center1.x, MIN(center2.x, center1.x + size1.x));
    float closest_y = MAX(center1.y, MIN(center2.y, center1.y + size1.y));
    float closest_z = MAX(center1.z, MIN(center2.z, center1.z + size1.z));

    float distance = sqrt(((closest_x - center2.x) * (closest_x - center2.x)) +
                          ((closest_y - center2.y) * (closest_y - center2.y)) +
                          ((closest_z - center2.z) * (closest_z - center2.z)) );

    return distance < radius;
}

inline bool test_sphere_sphere_collision(const sVector3  center1,
                                         const float  radius1,
                                         const sVector3  center2,
                                         const float     radius2) {
    float center_distance = sqrt(((center1.x - center2.x) * (center1.x - center2.x)) +
                                 ((center1.y - center2.y) * (center1.y - center2.y)) +
                                 ((center1.z - center2.z) * (center1.z - center2.z)));
    return center_distance < (radius1 + radius2);
}


#endif //QUEST_DEMO_COLLIDER_CONTROLLER_H
