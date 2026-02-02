#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

GLuint compileShader(GLuint type, const char* source);
GLuint createShaderProgram();