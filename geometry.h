#pragma once
#include "math.h"
#include <glad/glad.h>
#include <vector>

extern const float cubeVertices[];
extern const int cubeVertexCount;

void generateSphereMesh(int stacks, int sectors,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices);

GLuint createCubeVAO(GLuint& cubeVBO);

GLuint createTrajVAO(GLuint& trajVBO, std::vector<float> trajVerts);

GLuint createSphereVAO(int stacks, int sectors,
    GLuint& sphereVBO, GLuint& sphereEBO,
    std::vector<float>& sphereVerts,
    std::vector<unsigned int>& sphereIndices);
