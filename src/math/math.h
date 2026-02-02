#pragma once
#include <cmath>

struct Vec3 {
    float x{}, y{}, z{};
};

struct Mat4 {
    float m[16];
};

Mat4 identity();
Mat4 multiply(const Mat4& a, const Mat4& b);
Mat4 translate(const Vec3& t);
Mat4 perspective(float fovRad, float aspect, float nearZ, float farZ);
Mat4 scale(float s);
Mat4 rotateAxisAngle(float angleRad, Vec3 axis);
Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);