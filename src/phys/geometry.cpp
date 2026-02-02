#include "geometry.h"

const float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
};

const int cubeVertexCount = sizeof(cubeVertices) / sizeof(float);

void generateSphereMesh(int stacks, int sectors,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices)
{
    vertices.clear();
    indices.clear();
    const float PI = 3.14159265358979323846f;

    for (int i = 0; i <= stacks; ++i) {
        float v = (float)i / (float)stacks;      //[0,1]
        float phi = PI * v - PI / 2.0f;          //[-pi/2, pi/2]

        float cosPhi = std::cos(phi);
        float sinPhi = std::sin(phi);

        for (int j = 0; j <= sectors; ++j) {
            float u = (float)j / (float)sectors; //[0,1]
            float theta = 2.0f * PI * u;         //[0, 2pi]

            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            float x = cosPhi * cosTheta;
            float y = sinPhi;
            float z = cosPhi * sinTheta;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // index
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int row1 = i * (sectors + 1);
            int row2 = (i + 1) * (sectors + 1);

            unsigned int i1 = row1 + j;
            unsigned int i2 = row1 + j + 1;
            unsigned int i3 = row2 + j;
            unsigned int i4 = row2 + j + 1;

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);

            indices.push_back(i2);
            indices.push_back(i4);
            indices.push_back(i3);
        }
    }
}

GLuint createCubeVAO(GLuint& cubeVBO) {
    GLuint cubeVAO = 0;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertexCount * sizeof(float), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
   
    return cubeVAO;
}

GLuint createTrajVAO(GLuint& trajVBO, std::vector<float> trajVerts ) {
    GLuint trajVAO = 0;
    
    glGenVertexArrays(1, &trajVAO);
    glGenBuffers(1, &trajVBO);
    glBindVertexArray(trajVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trajVBO);
    glBufferData(GL_ARRAY_BUFFER, trajVerts.size() * sizeof(float), trajVerts.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return trajVAO;
}

GLuint createSphereVAO(int stacks, int sectors,
    GLuint& sphereVBO, GLuint& sphereEBO,
    std::vector<float>& sphereVerts,
    std::vector<unsigned int>& sphereIndices) {
    generateSphereMesh(stacks, sectors, sphereVerts, sphereIndices);

    GLuint sphereVAO = 0;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(float),
                 sphereVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int),
                 sphereIndices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    return sphereVAO;
}
