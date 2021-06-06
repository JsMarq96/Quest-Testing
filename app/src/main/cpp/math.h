//
// Created by jsmar on 20/05/2021.
//
#ifndef __MATH_H_
#define __MATH_H_

#include <string.h>
#include <assert.h>
#include <math.h>

/**
 * Single header Math library with usefull utilities and structures
 * by Juan S. Marquerie
 * */


inline float abs_diff(const float  x,
                      const float  y) {
    return (x > y) ? x - y : y - x;
}
struct sQuaternion4;
union sMat44;
void convert_quaternion_to_matrix(const sQuaternion4 *quat, sMat44 *mat);

union sVector2 {
    struct { float x = 0.0f; float y = 0.0f;};
    float raw_values[2];
};

union sVector3 {
    struct { float x = 0.0f; float y = 0.0f; float z = 0.0f; };
    float raw_values[3];
};

union sVector4 {
    struct { float x = 0.0f; float y = 0.0f; float z = 0.0f; float w = 0.0f; };
    float raw_values[4];

    sVector4(){};
};

struct sRect2 {
    sVector2    point;
    float       width     = 1.0f;
    float       height    = 1.0f;
};

union sMat33 {
    float raw_values[9];
    struct {
        float sx1 = 1.0f; float sy1 = 0.0f; float px = 0.0f;
        float sx2 = 0.0f; float sy2 = 1.0f; float py = 0.0f;
        float tmp1 = 0.0f; float tmp2 = 0.0f; float tmp3 = 1.0;
    };

    sMat33() {
        sx1 = 1.0f;  sy1 = 0.0f;  px = 0.0f;
        sx2 = 0.0f;  sy2 = 1.0f;  py = 0.0f;
        tmp1 = 0.0f; tmp2 = 0.0f; tmp3 = 1.0;
    }

    inline sVector2 multiply(const sVector2   *vect) {
        return sVector2{ (vect->x * sx1) + (vect->y * sy1) + px, (vect->x * sx2) + (vect->y * sy2) + py };
    }

    inline void set_position(const sVector2    vec) {
        px = vec.x;
        py = vec.y;
    }

    inline void set_scale(const sVector2   vec) {
        sx1 = vec.x;
        sy2 = vec.y;
    }
};

struct sQuaternion4 {
    float q0 = 0.0f;
    float q1 = 0.0f;
    float q2 = 0.0f;
    float q3 = 0.0f;
};

union sMat44 {
    struct {
        float sx1 = 1.0f; float sy1 = 0.0f; float sz1 = 0.0f; float tmp1 = 0.0f;
        float sx2 = 0.0f; float sy2 = 1.0f; float sz2 = 0.0f; float tmp2 = 0.0f;
        float sx3 = 0.0f; float sy3 = 0.0f; float sz3 = 1.0f; float tmp3 = 0.0f;
        float px = 0.0f; float py = 0.0f; float pz = 0.0f; float tmp4 = 1.0f;
    };
    float raw_values[16];
    float mat_values[4][4];

    sMat44() {
        set_identity();
    }

    void
    set_identity() {
        sx1 = 1.0f; sy1 = 0.0f; sz1 = 0.0f; tmp1 = 0.0f;
        sx2 = 0.0f; sy2 = 1.0f; sz2 = 0.0f; tmp2 = 0.0f;
        sx3 = 0.0f; sy3 = 0.0f; sz3 = 1.0f; tmp3 = 0.0f;
        px = 0.0f; py = 0.0f; pz = 0.0f; tmp4 = 1.0f;
    }

    ///
    /// SIMPLE OPERATIONS
    ///

    inline void set_position(const sVector3 vec) {
        px = vec.x;
        py = vec.y;
        pz = vec.z;
    }

    inline void set_scale(const sVector3 vec) {
        sx1 = vec.x;
        sy2 = vec.y;
        sz3 = vec.z;
    }


    ///
    /// COMPLEX OPERATIONS
    ///

    // TODO: Wrap loops
    // TODO: Optimize via SIMD
    inline void
    multiply(const sMat44   *B) {
        sMat44 result;
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                float tmp = 0.0f;
                for (int i = 0; i < 4; i++) {
                    tmp += mat_values[x][i] * B->mat_values[i][y];
                }

                result.mat_values[x][y] = tmp;
            }
        }

        memcpy(mat_values, result.mat_values, sizeof(float) * 16);
    }

    inline void
    rotate(const sQuaternion4 *quat) {
        sMat44 matrix;
        convert_quaternion_to_matrix(quat, &matrix);
        multiply(&matrix);
    }

    inline sVector4 multiply(const sVector4   vect) {
        sVector4 result;
        for (int i = 0; i < 4; i++) {
            result.raw_values[i] = (vect.raw_values[0] * mat_values[i][0]) +
                                   (vect.raw_values[1] * mat_values[i][1]) +
                                   (vect.raw_values[2] * mat_values[i][2]) +
                                   (vect.raw_values[3] * mat_values[i][3]);
        }

        return result;
    }

    // Yoinked from a stackoverlof that yoinked from the MESA implmentation
    // of GLU
    // https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
    // It uses Sarrus' rule
    // TODO: SIMD...?
    void invert(sMat44 *result) const {
        float inv[16], det;
        int i;

        inv[0] = raw_values[5]  * raw_values[10] * raw_values[15] -
                 raw_values[5]  * raw_values[11] * raw_values[14] -
                 raw_values[9]  * raw_values[6]  * raw_values[15] +
                 raw_values[9]  * raw_values[7]  * raw_values[14] +
                 raw_values[13] * raw_values[6]  * raw_values[11] -
                 raw_values[13] * raw_values[7]  * raw_values[10];

        inv[4] = -raw_values[4]  * raw_values[10] * raw_values[15] +
                 raw_values[4]  * raw_values[11] * raw_values[14] +
                 raw_values[8]  * raw_values[6]  * raw_values[15] -
                 raw_values[8]  * raw_values[7]  * raw_values[14] -
                 raw_values[12] * raw_values[6]  * raw_values[11] +
                 raw_values[12] * raw_values[7]  * raw_values[10];

        inv[8] = raw_values[4]  * raw_values[9] * raw_values[15] -
                 raw_values[4]  * raw_values[11] * raw_values[13] -
                 raw_values[8]  * raw_values[5] * raw_values[15] +
                 raw_values[8]  * raw_values[7] * raw_values[13] +
                 raw_values[12] * raw_values[5] * raw_values[11] -
                 raw_values[12] * raw_values[7] * raw_values[9];

        inv[12] = -raw_values[4]  * raw_values[9] * raw_values[14] +
                  raw_values[4]  * raw_values[10] * raw_values[13] +
                  raw_values[8]  * raw_values[5] * raw_values[14] -
                  raw_values[8]  * raw_values[6] * raw_values[13] -
                  raw_values[12] * raw_values[5] * raw_values[10] +
                  raw_values[12] * raw_values[6] * raw_values[9];

        inv[1] = -raw_values[1]  * raw_values[10] * raw_values[15] +
                 raw_values[1]  * raw_values[11] * raw_values[14] +
                 raw_values[9]  * raw_values[2] * raw_values[15] -
                 raw_values[9]  * raw_values[3] * raw_values[14] -
                 raw_values[13] * raw_values[2] * raw_values[11] +
                 raw_values[13] * raw_values[3] * raw_values[10];

        inv[5] = raw_values[0]  * raw_values[10] * raw_values[15] -
                 raw_values[0]  * raw_values[11] * raw_values[14] -
                 raw_values[8]  * raw_values[2] * raw_values[15] +
                 raw_values[8]  * raw_values[3] * raw_values[14] +
                 raw_values[12] * raw_values[2] * raw_values[11] -
                 raw_values[12] * raw_values[3] * raw_values[10];

        inv[9] = -raw_values[0]  * raw_values[9] * raw_values[15] +
                 raw_values[0]  * raw_values[11] * raw_values[13] +
                 raw_values[8]  * raw_values[1] * raw_values[15] -
                 raw_values[8]  * raw_values[3] * raw_values[13] -
                 raw_values[12] * raw_values[1] * raw_values[11] +
                 raw_values[12] * raw_values[3] * raw_values[9];

        inv[13] = raw_values[0]  * raw_values[9] * raw_values[14] -
                  raw_values[0]  * raw_values[10] * raw_values[13] -
                  raw_values[8]  * raw_values[1] * raw_values[14] +
                  raw_values[8]  * raw_values[2] * raw_values[13] +
                  raw_values[12] * raw_values[1] * raw_values[10] -
                  raw_values[12] * raw_values[2] * raw_values[9];

        inv[2] = raw_values[1]  * raw_values[6] * raw_values[15] -
                 raw_values[1]  * raw_values[7] * raw_values[14] -
                 raw_values[5]  * raw_values[2] * raw_values[15] +
                 raw_values[5]  * raw_values[3] * raw_values[14] +
                 raw_values[13] * raw_values[2] * raw_values[7] -
                 raw_values[13] * raw_values[3] * raw_values[6];

        inv[6] = -raw_values[0]  * raw_values[6] * raw_values[15] +
                 raw_values[0]  * raw_values[7] * raw_values[14] +
                 raw_values[4]  * raw_values[2] * raw_values[15] -
                 raw_values[4]  * raw_values[3] * raw_values[14] -
                 raw_values[12] * raw_values[2] * raw_values[7] +
                 raw_values[12] * raw_values[3] * raw_values[6];

        inv[10] = raw_values[0]  * raw_values[5] * raw_values[15] -
                  raw_values[0]  * raw_values[7] * raw_values[13] -
                  raw_values[4]  * raw_values[1] * raw_values[15] +
                  raw_values[4]  * raw_values[3] * raw_values[13] +
                  raw_values[12] * raw_values[1] * raw_values[7] -
                  raw_values[12] * raw_values[3] * raw_values[5];

        inv[14] = -raw_values[0]  * raw_values[5] * raw_values[14] +
                  raw_values[0]  * raw_values[6] * raw_values[13] +
                  raw_values[4]  * raw_values[1] * raw_values[14] -
                  raw_values[4]  * raw_values[2] * raw_values[13] -
                  raw_values[12] * raw_values[1] * raw_values[6] +
                  raw_values[12] * raw_values[2] * raw_values[5];

        inv[3] = -raw_values[1] * raw_values[6] * raw_values[11] +
                 raw_values[1] * raw_values[7] * raw_values[10] +
                 raw_values[5] * raw_values[2] * raw_values[11] -
                 raw_values[5] * raw_values[3] * raw_values[10] -
                 raw_values[9] * raw_values[2] * raw_values[7] +
                 raw_values[9] * raw_values[3] * raw_values[6];

        inv[7] = raw_values[0] * raw_values[6] * raw_values[11] -
                 raw_values[0] * raw_values[7] * raw_values[10] -
                 raw_values[4] * raw_values[2] * raw_values[11] +
                 raw_values[4] * raw_values[3] * raw_values[10] +
                 raw_values[8] * raw_values[2] * raw_values[7] -
                 raw_values[8] * raw_values[3] * raw_values[6];

        inv[11] = -raw_values[0] * raw_values[5] * raw_values[11] +
                  raw_values[0] * raw_values[7] * raw_values[9] +
                  raw_values[4] * raw_values[1] * raw_values[11] -
                  raw_values[4] * raw_values[3] * raw_values[9] -
                  raw_values[8] * raw_values[1] * raw_values[7] +
                  raw_values[8] * raw_values[3] * raw_values[5];

        inv[15] = raw_values[0] * raw_values[5] * raw_values[10] -
                  raw_values[0] * raw_values[6] * raw_values[9] -
                  raw_values[4] * raw_values[1] * raw_values[10] +
                  raw_values[4] * raw_values[2] * raw_values[9] +
                  raw_values[8] * raw_values[1] * raw_values[6] -
                  raw_values[8] * raw_values[2] * raw_values[5];

        det = raw_values[0] * inv[0] + raw_values[1] * inv[4] + raw_values[2] * inv[8] + raw_values[3] * inv[12];

        assert(det != 0.0f && "Cannot inverse a matrix with 0 determinant");

        det = 1.0f / det;

        for (i = 0; i < 16; i++) {
            result->raw_values[i] = inv[i] * det;
        }
    }
};

inline void
convert_quaternion_to_matrix(const sQuaternion4 *quat, sMat44 *mat) {
    mat->mat_values[0][0] = 2.0f * (quat->q0 * quat->q0 + quat->q1 * quat->q1) - 1;
    mat->mat_values[1][0] = 2.0f * (quat->q1 * quat->q1 + quat->q0 * quat->q3);
    mat->mat_values[2][0] = 2.0f * (quat->q1 * quat->q3 - quat->q0 * quat->q2);

    mat->mat_values[0][1] = 2.0f * (quat->q1 * quat->q2 - quat->q0 * quat->q3);
    mat->mat_values[1][1] = 2.0f * (quat->q0 * quat->q0 + quat->q2 * quat->q2) - 1;
    mat->mat_values[2][1] = 2.0f * (quat->q2 * quat->q3 + quat->q0 * quat->q1);

    mat->mat_values[0][2] = 2.0f * (quat->q1 * quat->q3 + quat->q0 * quat->q2);
    mat->mat_values[1][2] = 2.0f * (quat->q2 * quat->q3 - quat->q0 * quat->q1);
    mat->mat_values[2][2  ] = 2.0f * (quat->q0 * quat->q0 + quat->q3 * quat->q3) - 1;
}

#endif // __MATH_H_