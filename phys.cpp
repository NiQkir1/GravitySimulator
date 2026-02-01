#include "phys.h"

float distance3(const Vec3& a, const Vec3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void recordTrajectoryPoint(Body& b) {
    if (b.mass < 1e-6f) return;

    b.trajectoryStep++;

    bool shouldAdd = false;
    if (b.trajectory.empty()) {
        shouldAdd = true;
    }
    else {
        float dist = distance3(b.pos, b.trajectory.back());
        if (dist >= b.trajectoryMinDist) shouldAdd = true;
    }

    if (b.trajectoryStep % std::max(1, b.trajectoryStride) == 0) {
        shouldAdd = true;
    }

    if (!shouldAdd) return;

    b.trajectory.push_back(b.pos);
    if (b.trajectory.size() > Body::maxTrajectoryPoints) {
        b.trajectory.erase(b.trajectory.begin());
    }
}

void handleCollisions(std::vector<Body>& bodies, float dt, bool elastic) {
    const std::size_t n = bodies.size();
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            Body& b1 = bodies[i];
            Body& b2 = bodies[j];

            if (b1.isStatic && b2.isStatic) continue;

            float dx = b2.pos.x - b1.pos.x;
            float dy = b2.pos.y - b1.pos.y;
            float dz = b2.pos.z - b1.pos.z;
            float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
            float minDist = b1.radius + b2.radius;

            if (dist < minDist && dist > 1e-6f) {

                Vec3 normal{ dx / dist, dy / dist, dz / dist };

                if (elastic) {
                    float v1n = b1.vel.x * normal.x + b1.vel.y * normal.y + b1.vel.z * normal.z;
                    float v2n = b2.vel.x * normal.x + b2.vel.y * normal.y + b2.vel.z * normal.z;

                    float m1 = b1.mass;
                    float m2 = b2.mass;
                    float v1n_new = (v1n * (m1 - m2) + 2.0f * m2 * v2n) / (m1 + m2);
                    float v2n_new = (v2n * (m2 - m1) + 2.0f * m1 * v1n) / (m1 + m2);

                    b1.vel.x += (v1n_new - v1n) * normal.x;
                    b1.vel.y += (v1n_new - v1n) * normal.y;
                    b1.vel.z += (v1n_new - v1n) * normal.z;

                    b2.vel.x += (v2n_new - v2n) * normal.x;
                    b2.vel.y += (v2n_new - v2n) * normal.y;
                    b2.vel.z += (v2n_new - v2n) * normal.z;

                    float penetration = minDist - dist;
                    b1.pos.x -= normal.x * penetration * (m2 / (m1 + m2));
                    b1.pos.y -= normal.y * penetration * (m2 / (m1 + m2));
                    b1.pos.z -= normal.z * penetration * (m2 / (m1 + m2));

                    b2.pos.x += normal.x * penetration * (m1 / (m1 + m2));
                    b2.pos.y += normal.y * penetration * (m1 / (m1 + m2));
                    b2.pos.z += normal.z * penetration * (m1 / (m1 + m2));

                    b1.collisionFlash = 1.0f;
                    b2.collisionFlash = 1.0f;
                }
                else {
                    Body* larger = (b1.mass > b2.mass) ? &b1 : &b2;
                    Body* smaller = (b1.mass > b2.mass) ? &b2 : &b1;

                    float totalMass = larger->mass + smaller->mass;
                    larger->vel.x = (larger->vel.x * larger->mass + smaller->vel.x * smaller->mass) / totalMass;
                    larger->vel.y = (larger->vel.y * larger->mass + smaller->vel.y * smaller->mass) / totalMass;
                    larger->vel.z = (larger->vel.z * larger->mass + smaller->vel.z * smaller->mass) / totalMass;

                    larger->mass = totalMass;
                    float newRadius = std::max(larger->radius, smaller->radius) * 1.15f;
                    larger->radius = newRadius;
                    larger->visualRadius = newRadius;

                    larger->collisionFlash = 1.0f;

                    smaller->isStatic = true;
                    smaller->pos = { 1e10f, 1e10f, 1e10f };
                    smaller->mass = 0.0f;
                }
            }
        }
    }
}

void updatePhysics(std::vector<Body>& bodies, float dt, float G, float maxInteractionRadius) {
    for (auto& b : bodies) {
        b.acc = { 0.0f, 0.0f, 0.0f };
        //fading flash
        if (b.collisionFlash > 0.0f) {
            b.collisionFlash -= b.collisionFlashDecay * dt;
            if (b.collisionFlash < 0.0f) b.collisionFlash = 0.0f;
        }
    }

    const std::size_t n = bodies.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (bodies[i].mass < 1e-6f) continue;

        for (std::size_t j = 0; j < n; ++j) {
            if (i == j) continue;
            if (bodies[j].mass < 1e-6f) continue;

            Vec3 pi = bodies[i].pos;
            Vec3 pj = bodies[j].pos;

            float dx = pj.x - pi.x;
            float dy = pj.y - pi.y;
            float dz = pj.z - pi.z;

            float r2 = dx * dx + dy * dy + dz * dz;
            float r = std::sqrt(r2);

            if (r > maxInteractionRadius) continue;
            if (r < 1e-4f) continue;

            float a_mag_over_r = G * bodies[j].mass / (r2 * r);

            bodies[i].acc.x += a_mag_over_r * dx;
            bodies[i].acc.y += a_mag_over_r * dy;
            bodies[i].acc.z += a_mag_over_r * dz;
        }
    }

    for (auto& b : bodies) {
        if (b.isStatic || b.mass < 1e-6f) {
            b.rotationAngle += b.rotationSpeed * dt;
            if (b.rotationAngle > 2.0f * 3.14159265358979f)
                b.rotationAngle -= 2.0f * 3.14159265358979f;

            recordTrajectoryPoint(b);
            continue;
        }

        b.vel.x += b.acc.x * dt;
        b.vel.y += b.acc.y * dt;
        b.vel.z += b.acc.z * dt;

        b.pos.x += b.vel.x * dt;
        b.pos.y += b.vel.y * dt;
        b.pos.z += b.vel.z * dt;

        recordTrajectoryPoint(b);

        b.rotationAngle += b.rotationSpeed * dt;
        if (b.rotationAngle > 2.0f * 3.14159265358979f)
            b.rotationAngle -= 2.0f * 3.14159265358979f;
    }

    handleCollisions(bodies, dt);
}