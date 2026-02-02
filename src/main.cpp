/*
 * 3D Gravity Simulation

 * Func:
 * - 3D visual N-body sys
 * - Smooth animation of body motion
 * - Display of motion trajectories
 * - Camera control (WASD, space/space+shift, mouse, scroll)
 * - Collision handling with visual effects (flash)
 * - Support for arbitrary number of bodies
 * - Interaction optimization (radius limitation)
 */


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include "math.h"
#include "phys/phys.h"
#include "render/camera.h"
#include "render/shader.h"
#include "phys/geometry.h"
#include "render/shaderSetup.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Не удалось инициализировать GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Gravity Simulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Не удалось создать окно GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Не удалось инициализировать GLAD\n";
        glfwTerminate();
        return -1;
    }
    IMGUI_CHECKVERSION(); ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    glEnable(GL_DEPTH_TEST);

    ShaderUniforms uniforms; 
    GLuint shaderProgram = initShaderProgram(uniforms);

    GLuint cubeVBO;
    GLuint cubeVAO = createCubeVAO(cubeVBO);

    std::vector<float> sphereVerts;
    std::vector<unsigned int> sphereIndices;
    generateSphereMesh(16, 32, sphereVerts, sphereIndices);
    GLuint sphereVBO, sphereEBO;
    GLuint sphereVAO = createSphereVAO(16, 32, sphereVBO, sphereEBO, sphereVerts, sphereIndices);

    Mat4 projection = perspective(45.0f * 3.14159265358979f / 180.0f,
        800.0f / 600.0f, 0.1f, 100.0f);

    int winWidth = 800, winHeight = 600;

    const float G = 1.0f;
    float dt = 0.001f;

    // ------ init body ------
    std::vector<Body> bodies;

    //body
    Body cube{};
    cube.pos = { 0.0f, 0.0f, 0.0f };
    cube.vel = { 0.0f, 0.0f, 0.0f };
    cube.mass = 1000.0f;
    cube.shape = ShapeType::Cube;
    cube.type = BodyType::Star;
    cube.radius = 2.0f;
    cube.visualRadius = 2.0f;
    cube.color = { 0.9f, 0.7f, 0.2f };
    cube.isStatic = true;
    cube.rotationAxis = { 0.5f, 1.0f, 0.3f };
    cube.rotationSpeed = 2.0f;

    //body
    float R1 = 8.0f;
    float v1 = std::sqrt(G * cube.mass / R1);
    Body s1{};
    s1.pos = { R1, 0.0f, 0.0f };
    s1.vel = { 0.0f, v1, 1.0f };
    s1.mass = 10.0f;
    s1.shape = ShapeType::Sphere;
    s1.type = BodyType::Planet;
    s1.radius = 1.0f;
    s1.visualRadius = 1.0f;
    s1.color = { 0.3f, 0.7f, 0.9f };

    //body
    float R2 = 8.f;
    float v2 = std::sqrt(G * cube.mass / R2);
    Body s2{};
    s2.pos = { -R2, 0.0f, 0.0f };
    s2.vel = { 0.0f, -v2, 0.0f };
    s2.mass = 0.5f;
    s2.shape = ShapeType::Sphere;
    s2.type = BodyType::Planet;
    s2.radius = 0.8f;
    s2.visualRadius = 0.8f;
    s2.color = { 0.8f, 0.3f, 0.9f };

    bodies.push_back(cube);
    bodies.push_back(s1);
    bodies.push_back(s2);

    //body
    for (int i = 0; i < 5; ++i) {
        float angle = i * 1.1f * 3.14159265358979f / 5.0f;
        float R = 15.0f + i * 2.0f;
        float v = std::sqrt(G * s1.mass / R);
        Body ast{};
        ast.pos = { R * std::cos(angle), (i - 2.5f) * 1.5f, R * std::sin(angle) };
        ast.vel = { -v * std::sin(angle), 0.0f, v * std::cos(angle) };
        ast.mass = 0.2f + i * 0.1f;
        ast.shape = ShapeType::Cube;
        ast.type = BodyType::Asteroid;
        ast.radius = 0.3f + i * 0.1f;
        ast.visualRadius = ast.radius;
        ast.color = { 0.5f + i * 0.1f, 0.3f, 0.7f };
        bodies.push_back(ast);
    }

    float simTime = 0.0f;
    float lastFrameTime = static_cast<float>(glfwGetTime());

    Vec3 lastPos = camera.position;

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        ImGuiIO& io = ImGui::GetIO();

        processInput(window, deltaTime);

        glfwGetFramebufferSize(window, &winWidth, &winHeight);
        if (winWidth > 0 && winHeight > 0) {
            projection = perspective(camera.fov * 3.14159265358979f / 180.0f,
                static_cast<float>(winWidth) / static_cast<float>(winHeight),
                0.1f, 100.0f);
        }

        for (int i = 0; i < 5; ++i) {
            updatePhysics(bodies, dt, G);
            simTime += dt;
        }

        glClearColor(0.02f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Vec3 front = camera.getFront();
        Vec3 center{ camera.position.x + front.x,
                   camera.position.y + front.y,
                   camera.position.z + front.z };
        Vec3 up{ 0.0f, 1.0f, 0.0f };
        Mat4 view = lookAt(camera.position, center, up);

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(uniforms.viewLoc, 1, GL_FALSE, view.m);
        glUniformMatrix4fv(uniforms.projLoc, 1, GL_FALSE, projection.m);

        glDisable(GL_DEPTH_TEST);
        glUniform1f(uniforms.flashLoc, 0.0f);
        for (const auto& b : bodies) {
            if (b.trajectory.size() < 2 || b.mass < 1e-6f) continue;

            glUniform3f(uniforms.colorLoc, b.color.x * 0.5f, b.color.y * 0.5f, b.color.z * 0.5f);

            std::vector<float> trajVerts;
            for (const auto& p : b.trajectory) {
                trajVerts.push_back(p.x);
                trajVerts.push_back(p.y);
                trajVerts.push_back(p.z);
            }

            GLuint trajVAO, trajVBO;
            trajVAO = createTrajVAO(trajVBO, trajVerts);

            Mat4 trajModel = identity();
            glUniformMatrix4fv(uniforms.modelLoc, 1, GL_FALSE, trajModel.m);
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(b.trajectory.size()));

            glDeleteVertexArrays(1, &trajVAO);
            glDeleteBuffers(1, &trajVBO);
        }
        glEnable(GL_DEPTH_TEST);

        //draw body
        for (std::size_t i = 0; i < bodies.size(); ++i) {
            Body& b = bodies[i];
            if (b.mass < 1e-6f) continue;//if body removed

            Mat4 T = translate(b.pos);
            Mat4 S = scale(b.visualRadius);

            Mat4 R = identity();
            if (b.shape == ShapeType::Cube) {
                R = rotateAxisAngle(b.rotationAngle, b.rotationAxis);
            }

            Mat4 M = multiply(T, multiply(R, S));

            glUniformMatrix4fv(uniforms.modelLoc, 1, GL_FALSE, M.m);

            Vec3 finalColor = b.color;
            if (b.collisionFlash > 0.0f) {
                finalColor.x = std::min(1.0f, finalColor.x + b.collisionFlash);
                finalColor.y = std::min(1.0f, finalColor.y + b.collisionFlash);
                finalColor.z = std::min(1.0f, finalColor.z + b.collisionFlash);
            }
            glUniform3f(uniforms.colorLoc, finalColor.x, finalColor.y, finalColor.z);
            glUniform1f(uniforms.flashLoc, b.collisionFlash);

            if (b.shape == ShapeType::Cube) {
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            else {
                glBindVertexArray(sphereVAO);
                glDrawElements(GL_TRIANGLES,
                    static_cast<GLsizei>(sphereIndices.size()),
                    GL_UNSIGNED_INT, 0);
            }

        }

        //draw gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImVec2 displaySize = ImGui::GetIO().DisplaySize; 
        ImGui::SetNextWindowPos(ImVec2(displaySize.x - 400, 0)); 
        ImGui::SetNextWindowSize(ImVec2(400, displaySize.y)); 

        
        ImGui::Begin("Editor");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Camera: %.2f, %.2f, %.2f", camera.position.x, camera.position.y, camera.position.z);
        ImGui::SliderFloat("FOV", &camera.fov, 10.0f, 120.0f);
        ImGui::SliderFloat(("dt"), &dt, 0.00001f, 1.0f);
         
        for (size_t i = 0; i < bodies.size(); ++i) {
            Body& b = bodies[i];  
            {
                if (ImGui::CollapsingHeader(("Body " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen))
                {
                    ImGui::SliderFloat(("mass##" + std::to_string(i)).c_str(), &b.mass, 0.1f, 2000.0f);
                    ImGui::InputFloat3(("pos##" + std::to_string(i)).c_str(), &b.pos.x);
                    ImGui::InputFloat3(("vel##" + std::to_string(i)).c_str(), &b.vel.x);
                    ImGui::ColorEdit3(("col##" + std::to_string(i)).c_str(), &b.color.x);
                }
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteProgram(shaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}