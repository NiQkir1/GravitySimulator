#include "math.h"

Mat4 identity() {
    Mat4 mat{};
    for (int i = 0; i < 16; ++i) mat.m[i] = 0.0f;
    mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 1.0f;
    return mat;
}

Mat4 multiply(const Mat4& a, const Mat4& b) {
    Mat4 r{};
    for (int i = 0; i < 16; ++i) r.m[i] = 0.0f;
    // column-major: r = a * b
    for (int c = 0; c < 4; ++c) {
        for (int r0 = 0; r0 < 4; ++r0) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a.m[k * 4 + r0] * b.m[c * 4 + k];
            }
            r.m[c * 4 + r0] = sum;
        }
    }
    return r;
}

Mat4 translate(const Vec3& t) {
    Mat4 mat = identity();
    mat.m[12] = t.x;
    mat.m[13] = t.y;
    mat.m[14] = t.z;
    return mat;
}

Mat4 scale(float s) {
    Mat4 mat = identity();
    mat.m[0] = mat.m[5] = mat.m[10] = s;
    return mat;
}

Mat4 rotateAxisAngle(float angleRad, Vec3 axis) {
    float len = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (len < 1e-8f) return identity();
    axis.x /= len; axis.y /= len; axis.z /= len;

    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    float one = 1.0f - c;

    Mat4 m = identity();

    m.m[0] = c + axis.x * axis.x * one;
    m.m[1] = axis.y * axis.x * one - axis.z * s;
    m.m[2] = axis.z * axis.x * one + axis.y * s;

    m.m[4] = axis.x * axis.y * one + axis.z * s;
    m.m[5] = c + axis.y * axis.y * one;
    m.m[6] = axis.z * axis.y * one - axis.x * s;

    m.m[8] = axis.x * axis.z * one - axis.y * s;
    m.m[9] = axis.y * axis.z * one + axis.x * s;
    m.m[10] = c + axis.z * axis.z * one;

    return m;
}

Mat4 perspective(float fovRad, float aspect, float nearZ, float farZ) {
    Mat4 mat{};
    for (int i = 0; i < 16; ++i) mat.m[i] = 0.0f;
    float tanHalf = std::tan(fovRad / 2.0f);
    mat.m[0] = 1.0f / (aspect * tanHalf);
    mat.m[5] = 1.0f / tanHalf;
    mat.m[10] = -(farZ + nearZ) / (farZ - nearZ);
    mat.m[11] = -1.0f;
    mat.m[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
    return mat;
}

Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f{ center.x - eye.x, center.y - eye.y, center.z - eye.z };
    float lenF = std::sqrt(f.x * f.x + f.y * f.y + f.z * f.z);
    if (lenF > 1e-6f) {
        f.x /= lenF; f.y /= lenF; f.z /= lenF;
    }

    Vec3 s{};
    s.x = f.y * up.z - f.z * up.y;
    s.y = f.z * up.x - f.x * up.z;
    s.z = f.x * up.y - f.y * up.x;
    float lenS = std::sqrt(s.x * s.x + s.y * s.y + s.z * s.z);
    if (lenS > 1e-6f) {
        s.x /= lenS; s.y /= lenS; s.z /= lenS;
    }

    Vec3 u{};
    u.x = s.y * f.z - s.z * f.y;
    u.y = s.z * f.x - s.x * f.z;
    u.z = s.x * f.y - s.y * f.x;

    Mat4 result{};
    result.m[0] = s.x;
    result.m[1] = u.x;
    result.m[2] = -f.x;
    result.m[3] = 0.0f;

    result.m[4] = s.y;
    result.m[5] = u.y;
    result.m[6] = -f.y;
    result.m[7] = 0.0f;

    result.m[8] = s.z;
    result.m[9] = u.z;
    result.m[10] = -f.z;
    result.m[11] = 0.0f;

    result.m[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
    result.m[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
    result.m[14] = -(-f.x * eye.x - f.y * eye.y - f.z * eye.z);
    result.m[15] = 1.0f;

    return result;
}