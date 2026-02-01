#pragma once
#include "math.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

struct Camera {
    Vec3 position{ 0.0f, 0.0f, 25.0f };
    float yaw{ -90.0f };      
    float pitch{ 0.0f };     
    float speed{ 15.0f };     //move speed
    float mouseSensitivity{ 0.1f };
    float fov{ 45.0f };
    float minFov{ 10.0f };
    float maxFov{ 120.0f };

    Vec3 getFront() const;
    Vec3 getRight() const;
};

extern Camera camera;
extern bool firstMouse;
extern float lastMouseX;
extern float lastMouseY;

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, float deltaTime);