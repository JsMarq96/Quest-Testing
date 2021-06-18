//
// Created by jsmar on 16/06/2021.
//

#ifndef QUEST_DEMO_SAT_COLLISION_TESTING_H
#define QUEST_DEMO_SAT_COLLISION_TESTING_H

#include <float.h>

#include "math.h"

#include <iostream>

inline void get_OBB_raw_vertex(const sVector3 obb_center,
                               const sVector3 obb_size,
                               const sQuaternion4 obb_rotation,
                               sVector3 *result) {
    result[0] = sVector3{};
    result[1] = {obb_size.x, 0.0f, 0.0f};
    result[2] = {0.0f, obb_size.y, 0.0f};
    result[3] = {obb_size.x, obb_size.x, 0.0f};
    result[4] = {0.0f, 0.0f, obb_size.z};
    result[5] = {0.0f, obb_size.y, obb_size.z};
    result[6] = {obb_size.x, 0.0f, obb_size.z};
    result[7] = {obb_size.x, obb_size.y, obb_size.z};

    for(int i = 0; i < 8; i++) {
        result[i] = rotate_vector3(result[i], obb_rotation);
        result[i] = { result[i].x + obb_center.x,
                      result[i].y + obb_center.y,
                      result[i].z + obb_center.z };
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
    float sum = max_1 - min_1 + max_2 - min_2;

    return sum >= total_span;
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

    sVector3 norm1_x = rotate_vector3(sVector3{1.0f, 0.0f, 0.0f}, obb1_rotation);
    sVector3 norm1_y = rotate_vector3(sVector3{0.0f, 1.0f, 0.0f}, obb1_rotation);
    sVector3 norm1_z = rotate_vector3(sVector3{0.0f, 0.0f, 1.0f}, obb1_rotation);
    sVector3 norm2_x = rotate_vector3(sVector3{1.0f, 0.0f, 0.0f}, obb2_rotation);
    sVector3 norm2_y = rotate_vector3(sVector3{0.0f, 1.0f, 0.0f}, obb2_rotation);
    sVector3 norm2_z = rotate_vector3(sVector3{0.0f, 0.0f, 1.0f}, obb2_rotation);

    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       norm1_x)) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       norm1_y)) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       norm1_z)) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       norm2_x)) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       norm2_y)) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       norm2_z)) {
        return false;
    }


    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_x, norm2_x))) {
                                           std::cout << "6" << std::endl;
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_x, norm2_y))) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_x, norm2_z))) {
        return false;
    }


    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_y, norm2_x))) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_y, norm2_y))) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_y, norm2_z))) {
        return false;
    }

    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_z, norm2_x))) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_z, norm2_y))) {
        return false;
    }
    if(!intersect_vertex_group_on_axis(obb1_vertex,
                                       obb2_vertex,
                                       cross_prod(norm1_z, norm2_z))) {
        return false;
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
