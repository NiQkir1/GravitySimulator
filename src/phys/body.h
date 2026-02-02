#pragma once
#include "../math/math.h"
#include <vector>

enum class ShapeType { Cube, Sphere };
enum class BodyType { Star, Planet, Asteroid };

struct Body {
    Vec3 pos{};
    Vec3 vel{};
    Vec3 acc{};
    float mass{};
    ShapeType shape{};
    BodyType type{ BodyType::Planet };
    float radius{};
    float visualRadius{};
    Vec3 color{ 0.8f, 0.8f, 0.8f };
    bool isStatic{ false };
    float rotationAngle{ 0.0f };
    Vec3 rotationAxis{ 0.0f, 1.0f, 0.0f };
    float rotationSpeed{ 1.0f };

    //traj
    std::vector<Vec3> trajectory{};
    static constexpr size_t maxTrajectoryPoints = 500;
    int trajectoryStep{ 0 };
    int trajectoryStride{ 3 };
    float trajectoryMinDist{ 0.15f };

    //collision effect
    float collisionFlash{ 1.0f }; //[0,1]
    float collisionFlashDecay{ 5.0f };
};
