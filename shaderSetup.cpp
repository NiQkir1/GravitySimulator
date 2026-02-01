#include "shaderSetup.h"
#include "shader.h"

GLuint initShaderProgram(ShaderUniforms& uniforms) {
    GLuint shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    uniforms.modelLoc = glGetUniformLocation(shaderProgram, "model");
    uniforms.viewLoc = glGetUniformLocation(shaderProgram, "view");
    uniforms.projLoc = glGetUniformLocation(shaderProgram, "projection");
    uniforms.colorLoc = glGetUniformLocation(shaderProgram, "color");
    uniforms.flashLoc = glGetUniformLocation(shaderProgram, "flashIntensity");

    return shaderProgram;
}
