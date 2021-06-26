//
// Created by jsmar on 07/06/2021.
//

#ifndef QUEST_DEMO_COLLISION_CONTROLLER_H
#define QUEST_DEMO_COLLISION_CONTROLLER_H

#include <VrApi.h>
#include "../utils/math.h"

#include "../utils/math.h"

#include "collision_types.h"
#include "mesh_renderer.h"
#include "SAT_collision_testing.h"

#define MAX_SCENE_COLLIDERS 100
#define TAG_SIZE 4

/**
 * Collider utility.
 * Collision resolution will come later
 * A collider can be entangles to a scene object, and move and rotate with it
 * */

enum eColliderType : int {
    SPHERE_COLLIDER = 0,
    OBB_COLLIDER
};

enum eColliderAttributes : unsigned char {
    IS_TRIGGER = 0b00001,
    IS_STATIC  = 0b00010
};

#define DEBUG TRUE

struct sColliderController {
    /// POINTER THE CURRENT SCENE'S DATA
    sVector3       *position;
    sQuaternion4   *rotation;
    bool           *enabled;

    /// COLLIDER BEHAVIOUR
    bool           enabled_colliders       [MAX_SCENE_COLLIDERS] = { false };
    int            object_entanglement     [MAX_SCENE_COLLIDERS] = { -1 };
    unsigned char  attributes              [MAX_SCENE_COLLIDERS] = { 0b000 };
    eColliderType  collider_type           [MAX_SCENE_COLLIDERS] = { OBB_COLLIDER };

    /// COLLIDER POSITION, SIZE & SHAPE
    sVector3       origin_points           [MAX_SCENE_COLLIDERS] = { sVector3{0.0f, 0.f, 0.0f} };
    sVector3       scale                   [MAX_SCENE_COLLIDERS] = { sVector3{0.0f, 0.f, 0.0f} };
    sMat44         box_transform           [MAX_SCENE_COLLIDERS] = {  };
    float          sphere_collider_radius  [MAX_SCENE_COLLIDERS] = { 0.0f };
    sVector3       collider_raw_vertex     [MAX_SCENE_COLLIDERS][15] = { 0.0f };

    /// MISC COLLIDER INFO
    char           collider_tag[TAG_SIZE]  [MAX_SCENE_COLLIDERS] = {""};



    // TODO: Add labels for the collisions
    // TODO: Add triggers for collisions

    sMeshRenderer debug_renderer;
    // TODO add mesh renderers for sphere colliders
};

//// LIFECYCLE FUNCTIONS
void CC_init(sColliderController *col_contr,
             bool                *enabled_objs,
             sVector3            *obj_positions,
             sQuaternion4        *obj_rotation);

void CC_update(sColliderController    *col_contr,
               sCollisionManifold     *result_collisions,
               int                    *collision_count);

void CC_render(const sColliderController *col_contr,
               const ovrTracking2         *tracking,
               const unsigned int         eye_index);


//// COLLIDER MANAGING FUNCTIONS
int CC_add_OBB_collider(sColliderController *controller,
                        const sVector3       position,
                        const sQuaternion4   rotation,
                        const float          width,
                        const float          heigth,
                        const float          depth);

//// COLLISION TESTING FUNCTIONS
inline bool test_AABB_AABB_collision(const sVector3  aabb_center1,
                                     const sVector3  size1,
                                     const sVector3  aabb_center2,
                                     const sVector3  size2) {
    return (aabb_center1.x <= (aabb_center2.x + size2.x) && (aabb_center1.x + size1.x) >= aabb_center2.x) &&
           (aabb_center1.y <= (aabb_center2.y + size2.y) && (aabb_center1.y + size1.y) >= aabb_center2.y) &&
           (aabb_center1.z <= (aabb_center2.z + size2.z) && (aabb_center1.z + size1.z) >= aabb_center2.z);
}

inline bool test_AABB_sphere_collision(const sVector3  aabb_center,
                                       const sVector3  size1,
                                       const sVector3  sphere_center,
                                       const float     radius) {
    float closest_x = MAX(aabb_center.x, MIN(sphere_center.x, aabb_center.x + size1.x));
    float closest_y = MAX(aabb_center.y, MIN(sphere_center.y, aabb_center.y + size1.y));
    float closest_z = MAX(aabb_center.z, MIN(sphere_center.z, aabb_center.z + size1.z));

    float distance = sqrt(((closest_x - sphere_center.x) * (closest_x - sphere_center.x)) +
                          ((closest_y - sphere_center.y) * (closest_y - sphere_center.y)) +
                          ((closest_z - sphere_center.z) * (closest_z - sphere_center.z)) );

    return distance < radius;
}

inline bool test_sphere_sphere_collision(const sVector3  center1,
                                         const float     radius1,
                                         const sVector3  center2,
                                         const float     radius2) {
    float center_distance = sqrt(((center1.x - center2.x) * (center1.x - center2.x)) +
                                 ((center1.y - center2.y) * (center1.y - center2.y)) +
                                 ((center1.z - center2.z) * (center1.z - center2.z)));
    return center_distance < (radius1 + radius2);
}

inline bool test_sphere_point_collision(const sVector3  sphere_center,
                                        const float     radius,
                                        const sVector3  point) {
    float r2 = radius * radius;
    float center_distance = (((sphere_center.x - point.x) * (sphere_center.x - point.x)) +
                                 ((sphere_center.y - point.y) * (sphere_center.y - point.y)) +
                                 ((sphere_center.z - point.z) * (sphere_center.z - point.z)));
    return center_distance < r2;
}

inline bool test_AABB_point_collision(const sVector3 aabb_center,
                                      const sVector3 aabb_size,
                                      const sVector3 point) {
    return (point.x >= aabb_center.x && point.x <= (aabb_center.x + aabb_size.x)) &&
            (point.y >= aabb_center.y && point.y <= (aabb_center.y + aabb_size.y)) &&
            (point.z >= aabb_center.z && point.z <= (aabb_center.z + aabb_size.z));
}

inline bool test_AABB_OBB_collision(const sVector3  aabb_center,
                                    const sVector3  aabb_size,
                                    const sVector3  box_center,
                                    const sVector3  box_size,
                                    const sQuaternion4 box_rotation) {
    // Rotate AABB points to box rotation, and then peform an AABB test
    sVector3 p0 = aabb_center;
    sVector3 px{aabb_center.x + aabb_size.x, aabb_center.y, aabb_center.z};
    sVector3 py{aabb_center.x, aabb_center.y  + aabb_size.y, aabb_center.z};
    sVector3 pxy{aabb_center.x + aabb_size.x, aabb_center.y  + aabb_size.x, aabb_center.z};
    sVector3 pz{aabb_center.x, aabb_center.y, aabb_center.z + aabb_size.z};
    sVector3 pyz{aabb_center.x, aabb_center.y + aabb_size.y, aabb_center.z + aabb_size.z};
    sVector3 pxyz{aabb_center.x + aabb_size.x, aabb_center.y + aabb_size.y, aabb_center.z + aabb_size.z};
    sVector3 pxz{aabb_center.x + aabb_size.x, aabb_center.y, aabb_center.z + aabb_size.z};

    // Rotate points
    p0 = rotate_vector3(p0, box_rotation);
    px = rotate_vector3(px, box_rotation);
    py = rotate_vector3(py, box_rotation);
    pxy = rotate_vector3(pxy, box_rotation);
    pz = rotate_vector3(pz, box_rotation);
    pyz = rotate_vector3(pyz, box_rotation);
    pxyz = rotate_vector3(pxyz, box_rotation);
    pxz = rotate_vector3(pxz, box_rotation);

    return test_AABB_point_collision(box_center,
                                     box_size,
                                     p0) ||
           test_AABB_point_collision(box_center,
                                     box_size,
                                     px) ||
           test_AABB_point_collision(box_center,
                                     box_size,
                                     py) ||
           test_AABB_point_collision(box_center,
                                     box_size,
                                     pxy) ||
           test_AABB_point_collision(box_center,
                                     box_size,
                                     pz) ||
           test_AABB_point_collision(box_center,
                                     box_size,
                                     pyz) ||
           test_AABB_point_collision(box_center,
                                     box_size,
                                     pxyz) ||
           test_AABB_point_collision(box_center,
                                     box_size,
                                     pxz);
}

inline bool test_sphere_OBB_collision(const sVector3     sphere_center,
                                      const float        sphere_radius,
                                      const sVector3     box_center,
                                      const sVector3     box_size,
                                      const sQuaternion4 box_rotation) {
    // Declare points
    sVector3 p0 = box_center;
    sVector3 px{box_center.x + box_size.x, box_center.y, box_center.z};
    sVector3 py{box_center.x, box_center.y  + box_size.y, box_center.z};
    sVector3 pxy{box_center.x + box_size.x, box_center.y  + box_size.x, box_center.z};
    sVector3 pz{box_center.x, box_center.y, box_center.z + box_size.z};
    sVector3 pyz{box_center.x, box_center.y + box_size.y, box_center.z + box_size.z};
    sVector3 pxyz{box_center.x + box_size.x, box_center.y + box_size.y, box_center.z + box_size.z};
    sVector3 pxz{box_center.x + box_size.x, box_center.y + box_size.y, box_center.z + box_size.z};

    // Rotate points
    p0 = rotate_vector3(p0, box_rotation);
    px = rotate_vector3(px, box_rotation);
    py = rotate_vector3(py, box_rotation);
    pxy = rotate_vector3(pxy, box_rotation);
    pz = rotate_vector3(pz, box_rotation);
    pyz = rotate_vector3(pyz, box_rotation);
    pxyz = rotate_vector3(pxyz, box_rotation);
    pxz = rotate_vector3(pxz, box_rotation);

    return test_sphere_point_collision(sphere_center,
                                       sphere_radius,
                                       p0) ||
            test_sphere_point_collision(sphere_center,
                                        sphere_radius,
                                        px) ||
            test_sphere_point_collision(sphere_center,
                                        sphere_radius,
                                        pxy) ||
            test_sphere_point_collision(sphere_center,
                                        sphere_radius,
                                        py) ||
            test_sphere_point_collision(sphere_center,
                                        sphere_radius,
                                        pz) ||
            test_sphere_point_collision(sphere_center,
                                        sphere_radius,
                                        pyz) ||
            test_sphere_point_collision(sphere_center,
                                        sphere_radius,
                                        pxyz) ||
            test_sphere_point_collision(sphere_center,
                                        sphere_radius,
                                        pxz);
}


#endif //QUEST_DEMO_COLLISION_CONTROLLER_H
