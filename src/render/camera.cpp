#include "camera.h"

Vec3 Camera::getFront() const
{
        float yawRad = yaw * 3.14159265358979f / 180.0f;
        float pitchRad = pitch * 3.14159265358979f / 180.0f;
        Vec3 front{};
        front.x = std::cos(yawRad) * std::cos(pitchRad);
        front.y = std::sin(pitchRad);
        front.z = std::sin(yawRad) * std::cos(pitchRad);
        float len = std::sqrt(front.x * front.x + front.y * front.y + front.z * front.z);
        if (len > 1e-6f) {
            front.x /= len;
            front.y /= len;
            front.z /= len;
        }
        return front;
}
 
Vec3 Camera::getRight() const {
    Vec3 front = getFront();
    Vec3 worldUp{ 0.0f, 1.0f, 0.0f };
    Vec3 right{};
    right.x = front.y * worldUp.z - front.z * worldUp.y;
    right.y = front.z * worldUp.x - front.x * worldUp.z;
    right.z = front.x * worldUp.y - front.y * worldUp.x;
    float len = std::sqrt(right.x * right.x + right.y * right.y + right.z * right.z);
    if (len > 1e-6f) {
        right.x /= len;
        right.y /= len;
        right.z /= len;
    }
    return right;
}
   
Camera camera;
bool firstMouse = true;
float lastMouseX = 400.0f;
float lastMouseY = 300.0f;

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) return;
    if (firstMouse) {
        lastMouseX = static_cast<float>(xpos);
        lastMouseY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastMouseX;
    float yoffset = lastMouseY - static_cast<float>(ypos);
    lastMouseX = static_cast<float>(xpos);
    lastMouseY = static_cast<float>(ypos);

    xoffset *= camera.mouseSensitivity;
    yoffset *= camera.mouseSensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.fov -= static_cast<float>(yoffset) * 2.0f;
    if (camera.fov < camera.minFov)
        camera.fov = camera.minFov;
    if (camera.fov > camera.maxFov)
        camera.fov = camera.maxFov;
}

void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float velocity = camera.speed * deltaTime;
    Vec3 front = camera.getFront();
    Vec3 right = camera.getRight();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position = { camera.position.x + front.x * velocity,
                           camera.position.y + front.y * velocity,
                           camera.position.z + front.z * velocity };
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position = { camera.position.x - front.x * velocity,
                           camera.position.y - front.y * velocity,
                           camera.position.z - front.z * velocity };
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position = { camera.position.x - right.x * velocity,
                           camera.position.y - right.y * velocity,
                           camera.position.z - right.z * velocity };
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position = { camera.position.x + right.x * velocity,
                           camera.position.y + right.y * velocity,
                           camera.position.z + right.z * velocity };

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            camera.position.y -= velocity;
        }
        else {

            camera.position.y += velocity;
        }
    }
}