#pragma once
#include "body.h"

float distance3(const Vec3& a, const Vec3& b);
void recordTrajectoryPoint(Body& b);
void handleCollisions(std::vector<Body>& bodies, float dt, bool elastic = 0);
void updatePhysics(std::vector<Body>& bodies, float dt, float G, float maxInteractionRadius = 50.0f);
