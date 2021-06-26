//
// Created by jsmar on 16/06/2021.
//

#ifndef QUEST_DEMO_SAT_COLLISION_TESTING_H
#define QUEST_DEMO_SAT_COLLISION_TESTING_H

#include <float.h>

#include "../utils/math.h"
#include "../utils/geometry.h"

#include "collision_types.h"
#include "collision_controller.h"


inline void get_OBB_raw_vertex(const sMat44 transform,
                               sVector3 *result) {
    result[0] = sVector3{};
    result[1] = {1.0f, 0.0f, 0.0f};
    result[2] = {0.0f, 1.0f, 0.0f};
    result[3] = {1.0f, 1.0f, 0.0f};
    result[4] = {0.0f, 0.0f, 1.0f};
    result[5] = {0.0f, 1.0f, 1.0f};
    result[6] = {1.0f, 0.0f, 1.0f};
    result[7] = {1.0f, 1.0f, 1.0f};

    for(int i = 0; i < 8; i++) {
        result[i] = transform.multiply(result[i]);
    }
}


/*
int OBB_planes_indexing[6][4] = {
        {1, 2, 4, 5}, // face 0
        {4, 3, 2, 0}, // face 1
        {5, 3, 1, 0}, // face 2
        {5, 4, 7, 6}, // face 3
        {3, 5, 0, 1}, // face 4
        {4, 2, 3, 0}  // face 5
};

sVector3 OBB_normal_faces[] = {
        { 0.0f, 0.0f, -1.0f},
        { 1.0f, 0.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 1.0f},
        { 0.0f, -1.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f}
};


inline sPlane get_plane_from_obb(const sVector3 *vertex_list,
                                 const sVector3 *normal_list,
                                 const int face_id) {
    sPlane new_plane;
    sVector3 p1 = vertex_list[OBB_faces_indexing[face_id][0]];
    sVector3 p2 = vertex_list[OBB_faces_indexing[face_id][1]];
    sVector3 p3 = vertex_list[OBB_faces_indexing[face_id][2]];
    sVector3 p4 = vertex_list[OBB_faces_indexing[face_id][3]];

    new_plane.origin_point = sVector3{
            (p1.x + p2.x + p3.x + p4.x) / 4.0f,
            (p1.y + p2.y + p3.y + p4.y) / 4.0f,
            (p1.z + p2.z + p3.z + p4.z) / 4.0f
    };
    new_plane.normal = normal_list[face_id];

    return new_plane;
}*/


inline bool intersect_vertex_group_on_axis(const sVector3 obb1[8],
                                           const sVector3 obb2[8],
                                           const sVector3 axis,
                                           float          *diff) {
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

    *diff = sum - total_span;

    //std::cout << *diff << std::endl;

    return sum >= total_span;
}


inline bool SAT_OBB_v_OBB(const sMat44 obb1_transform,
                          const sQuaternion4 obb1_rotation,
                          const sMat44 obb2_transform,
                          const sQuaternion4 obb2_rotation,
                          sCollisionManifold  *result_manifold) {
    int OBB_faces_indexing[6][4] = {
            {0, 1, 2, 3},
            {6, 7, 3, 1},
            {2, 3, 7, 5},
            {5, 4, 7, 6},
            {4, 6, 0, 1},
            {5, 4, 2, 0}
    };
    sVector3 obb1_vertex[8] = {};
    sVector3 obb2_vertex[8] = {};

    sVector3 col_normal{};

    get_OBB_raw_vertex(obb1_transform,
                       &obb1_vertex[0]);
    get_OBB_raw_vertex(obb2_transform,
                       &obb2_vertex[0]);

    sVector3 norms_1[] = {
            rotate_vector3(sVector3{1.0f, 0.0f, 0.0f}, obb1_rotation),
            rotate_vector3(sVector3{0.0f, 1.0f, 0.0f}, obb1_rotation),
            rotate_vector3(sVector3{0.0f, 0.0f, 1.0f}, obb1_rotation),

            rotate_vector3(sVector3{-1.0f, 0.0f, 0.0f}, obb1_rotation),
            rotate_vector3(sVector3{0.0f, -1.0f, 0.0f}, obb1_rotation),
            rotate_vector3(sVector3{0.0f, 0.0f, -1.0f}, obb1_rotation),
    };

    sVector3 norms_2[] = {
            rotate_vector3(sVector3{1.0f, 0.0f, 0.0f}, obb2_rotation),
            rotate_vector3(sVector3{0.0f, 1.0f, 0.0f}, obb2_rotation),
            rotate_vector3(sVector3{0.0f, 0.0f, 1.0f}, obb2_rotation),

            rotate_vector3(sVector3{-1.0f, 0.0f, 0.0f}, obb2_rotation),
            rotate_vector3(sVector3{0.0f, -1.0f, 0.0f}, obb2_rotation),
            rotate_vector3(sVector3{0.0f, 0.0f, -1.0f}, obb2_rotation),
    };

    float min_diff = FLT_MAX;
    float diff = FLT_MAX;
    int col_case = -1;

    // Test all axis, since we need the face too
    // Test normals of OBB1
    for (int i = 0; i < 6; i++) {
        if(!intersect_vertex_group_on_axis(obb1_vertex,
                                           obb2_vertex,
                                           norms_1[i],
                                           &diff)) {
            return false;
        }

        if (min_diff > diff) {
            col_normal = norms_1[i];
            min_diff = diff;
            col_case = i;
        }
    }

    // Test normals of OBB2
    for (int i = 0; i < 6; i++) {
        if(!intersect_vertex_group_on_axis(obb1_vertex,
                                           obb2_vertex,
                                           norms_1[i],
                                           &diff)) {
            return false;
        }

        if (min_diff > diff) {
            col_normal = norms_1[i];
            min_diff = diff;
            col_case = i + 6;
        }
    }

    // TODO: test corners


    /// http://www.randygaul.net/2013/03/28/custom-physics-engine-part-2-manifold-generation/

    sVector3 reference_face[4] = {};
    sVector3 indent_face[4] = {};

    int reference_face_index = 0;
    int indent_face_index = -1;

    // Get the Incident face
    if (col_case < 6) {
        // The reference face is on OBB1
        float min_dot = FLT_MAX;
        float dot = 0.f;

        for(int i = 0; i < 6; i++) {
            dot = dot_prod(col_normal, norms_2[i]);
            if (dot < min_dot) {
                min_dot = dot;
                indent_face_index = i;
            }
        }

        reference_face_index = col_case;

        reference_face[0] = obb1_vertex[OBB_faces_indexing[reference_face_index][0]];
        reference_face[1] = obb1_vertex[OBB_faces_indexing[reference_face_index][1]];
        reference_face[2] = obb1_vertex[OBB_faces_indexing[reference_face_index][2]];
        reference_face[3] = obb1_vertex[OBB_faces_indexing[reference_face_index][3]];

        indent_face[0] = obb2_vertex[OBB_faces_indexing[indent_face_index][0]];
        indent_face[1] = obb2_vertex[OBB_faces_indexing[indent_face_index][1]];
        indent_face[2] = obb2_vertex[OBB_faces_indexing[indent_face_index][2]];
        indent_face[3] = obb2_vertex[OBB_faces_indexing[indent_face_index][3]];

    } else if (col_case < 12) {
        // The reference face is on OBB2
        float min_dot = FLT_MAX;
        float dot = 0.0f;

        for(int i = 0; i < 6; i++) {
            dot = dot_prod(col_normal, norms_1[i]);
            if (dot < min_dot) {
                min_dot = dot;
                indent_face_index = i;
            }
        }

        reference_face_index = col_case - 6;

        reference_face[0] = obb2_vertex[OBB_faces_indexing[reference_face_index][0]];
        reference_face[1] = obb2_vertex[OBB_faces_indexing[reference_face_index][1]];
        reference_face[2] = obb2_vertex[OBB_faces_indexing[reference_face_index][2]];
        reference_face[3] = obb2_vertex[OBB_faces_indexing[reference_face_index][3]];

        indent_face[0] = obb1_vertex[OBB_faces_indexing[indent_face_index][0]];
        indent_face[1] = obb1_vertex[OBB_faces_indexing[indent_face_index][1]];
        indent_face[2] = obb1_vertex[OBB_faces_indexing[indent_face_index][2]];
        indent_face[3] = obb1_vertex[OBB_faces_indexing[indent_face_index][3]];
    }

    int index = 0;

    sPlane refence_plane;

    refence_plane.origin_point = sVector3{
            (reference_face[0].x + reference_face[1].x + reference_face[2].x + reference_face[3].x) / 4.0f,
            (reference_face[0].y + reference_face[1].y + reference_face[2].y + reference_face[3].y) / 4.0f,
            (reference_face[0].z + reference_face[1].z + reference_face[2].z + reference_face[3].z) / 4.0f
    };

    refence_plane.normal = col_normal;

    // Skip clipping for OBBs... Lets see how it goes

    for(int i = 0; i < 4; i++) {
        float dist = refence_plane.distance(indent_face[i]);
        if (dist <= 0.0f) {
            result_manifold->points_depth[index] = dist;
            result_manifold->contact_points[index++] = indent_face[i];
        }
    }

    result_manifold->contact_point_count = index;
    result_manifold->collision_normal = col_normal;

    return true;
}

/*

https://ia801303.us.archive.org/30/items/GDC2013Gregorius/GDC2013-Gregorius.pdf
https://www.randygaul.net/2013/03/28/custom-physics-engine-part-2-manifold-generation/
https://gamedevelopment.tutsplus.com/tutorials/understanding-sutherland-hodgman-clipping-for-physics-engines--gamedev-11917
Numerical boutsness


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
