//
// Created by jsmar on 16/06/2021.
//

#ifndef QUEST_DEMO_SAT_COLLISION_TESTING_H
#define QUEST_DEMO_SAT_COLLISION_TESTING_H

#include <float.h>

#include "collision_controller.h"
#include "../utils/math.h"

inline void get_OBB_raw_vertex(const sVector3 obb_center,
                               const sVector3 obb_size,
                               const sQuaternion4 obb_rotation,
                               sVector3* result) {
    result[0] = obb_center;
    result[1] = {obb_center.x + obb_size.x, obb_center.y, obb_center.z};
    result[2] = {obb_center.x, obb_center.y  + obb_size.y, obb_center.z};
    result[3] = {obb_center.x + obb_size.x, obb_center.y  + obb_size.x, obb_center.z};
    result[4] = {obb_center.x, obb_center.y, obb_center.z + obb_size.z};
    result[5] = {obb_center.x, obb_center.y + obb_size.y, obb_center.z + obb_size.z};
    result[6] = {obb_center.x + obb_size.x, obb_center.y, obb_center.z + obb_size.z};
    result[7] = {obb_center.x + obb_size.x, obb_center.y + obb_size.y, obb_center.z + obb_size.z};

    for(int i = 0; i < 8; i++) {
        result[i] = rotate_vector3(result[i], obb_rotation);
    }
}

inline bool intersect_vertex_group_on_axis(const sVector3 obb1[8],
                                           const sVector3 obb2[8],
                                           const sVector3 axis) {
    float min_1 = FLT_MAX;
    float max_1 = FLT_MIN;
    float min_2 = FLT_MAX;
    float max_2 = FLT_MIN;

    if (axis.x == 0 && axis.y == 0 && axis.z == 0) {
        return true;
    }

    // Get the min and max position for each vertex group
    // proyected on teh vertex
    for (int i = 0; i < 8; i++) {
        float dist_1 = dot_prod(obb1[i], axis);
        float dist_2 = dot_prod(obb2[i], axis);
        min_1 = (dist_1 < min_1) ? dist_1 : min_1;
        max_1 = (dist_1 > max_1) ? dist_1 : max_1;
        min_2 = (dist_2 < min_2) ? dist_2 : min_2;
        max_2 = (dist_2 > max_2) ? dist_2 : max_2;
    }

    float total_span = MAX(max_1, max_2) - MIN(min_1, min_2);
    float sum = max_1 - min_1 + max_1 - max_2;

    return total_span <= sum;
}

inline void get_SAT_normals_from_raw_vertex_lists(const sVector3 obb1_origin,
                                                  const sQuaternion4 rotation1,
                                                  const sVector3 obb2_origin,
                                                  const sQuaternion4 rotation2,
                                                  sVector3 *result_normals) {

    result_normals[0] = sVector3{1.0f, 0.0f, 0.0f};
    result_normals[1] = sVector3{0.0f, 1.0f, 0.0f};
    result_normals[2] = sVector3{0.0f, 0.0f, 1.0f};
    result_normals[3] = sVector3{1.0f, 0.0f, 0.0f};
    result_normals[4] = sVector3{0.0f, 1.0f, 0.0f};
    result_normals[5] = sVector3{0.0f, 0.0f, 1.0f};
    result_normals[0] = rotate_vector3(sVector3{1.0f, 0.0f, 0.0f}, rotation1);
    result_normals[1] = rotate_vector3(sVector3{0.0f, 1.0f, 0.0f}, rotation1);
    result_normals[2] = rotate_vector3(sVector3{0.0f, 0.0f, 1.0f}, rotation1);
    result_normals[3] = rotate_vector3(sVector3{1.0f, 0.0f, 0.0f}, rotation2);
    result_normals[4] = rotate_vector3(sVector3{0.0f, 1.0f, 0.0f}, rotation2);
    result_normals[5] = rotate_vector3(sVector3{0.0f, 0.0f, 1.0f}, rotation2);

    /*result_normals[0] = sVector3{result_normals[0].x + obb1_origin.x,
                                 result_normals[0].y + obb1_origin.y,
                                 result_normals[0].z + obb1_origin.z};
    result_normals[1] = sVector3{result_normals[1].x + obb1_origin.x,
                                 result_normals[1].y + obb1_origin.y,
                                 result_normals[1].z + obb1_origin.z};
    result_normals[2] = sVector3{result_normals[2].x + obb1_origin.x,
                                 result_normals[2].y + obb1_origin.y,
                                 result_normals[2].z + obb1_origin.z};
    result_normals[3] = sVector3{result_normals[3].x + obb2_origin.x,
                                 result_normals[3].y + obb2_origin.y,
                                 result_normals[3].z + obb2_origin.z};
    result_normals[4] = sVector3{result_normals[4].x + obb2_origin.x,
                                 result_normals[4].y + obb2_origin.y,
                                 result_normals[4].z + obb2_origin.z};
    result_normals[5] = sVector3{result_normals[5].x + obb2_origin.x,
                                 result_normals[5].y + obb2_origin.y,
                                 result_normals[5].z + obb2_origin.z};*/

    result_normals[6]  = cross_prod(result_normals[0],result_normals[3]);
    result_normals[7]  = cross_prod(result_normals[0],result_normals[4]);
    result_normals[8]  = cross_prod(result_normals[0],result_normals[5]);
    result_normals[9]  = cross_prod(result_normals[1],result_normals[3]);
    result_normals[10] = cross_prod(result_normals[1],result_normals[4]);
    result_normals[11] = cross_prod(result_normals[1],result_normals[5]);
    result_normals[12] = cross_prod(result_normals[2],result_normals[3]);
    result_normals[13] = cross_prod(result_normals[2],result_normals[4]);
    result_normals[14] = cross_prod(result_normals[2],result_normals[5]);
}


inline bool SAT_OBB_v_OBB(const sVector3 obb1_origin,
                          const sVector3 obb1_sizes,
                          const sQuaternion4 obb1_rotation,
                          const sVector3 obb2_origin,
                          const sVector3 obb2_sizes,
                          const sQuaternion4 obb2_rotation) {
    sVector3 obb1_vertex[8] = {};
    sVector3 obb2_vertex[8] = {};
    sVector3 normal_list[15] = {};

    get_OBB_raw_vertex(obb1_origin,
                       obb1_sizes,
                       obb1_rotation,
                       &obb1_vertex[0]);
    get_OBB_raw_vertex(obb2_origin,
                       obb2_sizes,
                       obb2_rotation,
                       &obb2_vertex[0]);

    get_SAT_normals_from_raw_vertex_lists(obb1_origin,
                                          obb1_rotation,
                                          obb2_origin,
                                          obb2_rotation,
                                          &normal_list[0]);

    for (int i = 0; i < 15; i++) {
        if(!intersect_vertex_group_on_axis(obb1_vertex,
                                           obb2_vertex,
                                           normal_list[i])) {
            return false;
        }
    }

    return true;
}

/*
inline float get_axis_overlap(const float size1,
                              const float size2,
                              const float min_distance){
    return (size1 / 2.0f) + (size2 / 2.0f) - ABS(min_distance);
}

inline bool SAT_AABB_AABB_collision(const sVector3        aabb1_center,
                                    const sVector3        aabb1_size,
                                    const sVector3        aabb2_center,
                                    const sVector3        aabb2_size,
                                    sCollisionManifold    *result) {
    if (aabb1_center.x <= (aabb2_center.x + aabb2_size.x) && (aabb1_center.x + aabb1_size.x) >= aabb2_center.x) {
        return false;
    }
    if (aabb1_center.y <= (aabb2_center.y + aabb2_size.y) && (aabb1_center.y + aabb1_size.y) >= aabb2_center.y) {
        return false;
    }
    if (aabb1_center.z <= (aabb2_center.z + aabb2_size.z) && (aabb1_center.z + aabb1_size.z) >= aabb2_center.z) {
        return false;
    }

    // Fill collision manifold
    float overlap_x = get_axis_overlap(aabb1_size.x,
                                       aabb2_size.x,
                                       aabb1_center.x - aabb1_center.x);

    float overlap_y = get_axis_overlap(aabb1_size.y,
                                       aabb2_size.y,
                                       aabb1_center.y - aabb1_center.y);

    float overlap_z = get_axis_overlap(aabb1_size.z,
                                       aabb2_size.z,
                                       aabb1_center.z - aabb1_center.z);

    return true;
}*/

#endif //QUEST_DEMO_SAT_COLLISION_TESTING_H
