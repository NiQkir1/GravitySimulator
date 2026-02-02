#pragma once
#include <glad/glad.h>

struct ShaderUniforms {
    GLint modelLoc;
    GLint viewLoc;
    GLint projLoc;
    GLint colorLoc;
    GLint flashLoc;
};

GLuint initShaderProgram(ShaderUniforms& uniforms);
