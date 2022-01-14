#include "Sandbox.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <Core/RenderSystem.h>
#include <Core/Camera.h>
#include <Core/KeyboardController.h>

#include <stdexcept>
#include <array>
#include <chrono>

Sandbox::Sandbox() {
    LoadGameObjects();
}

Sandbox::~Sandbox() {
    
}

void Sandbox::Run() {
    RenderSystem renderSystem{ m_Device, m_Renderer.GetSwapChainRenderPass() };
    Camera camera{};
    camera.SetViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));

    auto viewer = GameObject::CreateGameObject();
    KeyboardController controller{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!m_Win.ShouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        controller.Move(m_Win.GetNativeWindow(), frameTime, viewer);
        camera.SetViewYXZ(viewer.transform.translation, viewer.transform.rotation);

        float aspect = m_Renderer.GetAspectRatio();
        //camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

        if (auto commandBuffer = m_Renderer.BeginFrame()) {
            m_Renderer.BeginSwapChainRenderPass(commandBuffer);
            renderSystem.RenderGameObjects(commandBuffer, m_GameObjects, camera);
            m_Renderer.EndSwapChainRenderPass(commandBuffer);
            m_Renderer.EndFrame();
        }
    }

    vkDeviceWaitIdle(m_Device.GetDevice());
}

//std::unique_ptr<Model> CreateCubeModel(Device& device, glm::vec3 offset) {
//    Model::Builder modelBuilder{};
//    modelBuilder.vertices = {
//        // left face (white)
//        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
//        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
//        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
//        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
//
//        // right face (yellow)
//        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
//        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
//        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
//        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
//
//        // top face (orange, remember y axis points down)
//        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
//        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
//        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
//        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
//
//        // bottom face (red)
//        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
//        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
//        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
//        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
//
//        // nose face (blue)
//        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
//        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
//        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
//        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
//
//        // tail face (green)
//        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
//        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
//        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
//        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
//    };
//    for (auto& v : modelBuilder.vertices) {
//        v.position += offset;
//    }
//
//    modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
//                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };
//
//    return std::make_unique<Model>(device, modelBuilder);
//}

void Sandbox::LoadGameObjects() {
    //std::vector<Model::Vertex> vertices{
    //    {{  0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    //    {{  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    //    {{ -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
    //};
    //Sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });

    std::shared_ptr<Model> smooth = Model::CreateModel(
        m_Device, 
        "C:\\dev\\VkTest\\VkTest\\src\\Resource\\models\\smooth_vase.obj");
    auto smoothVase = GameObject::CreateGameObject();
    smoothVase.model = smooth;
    smoothVase.transform.translation = { -0.5f, 0.0f, 2.5f };
    smoothVase.transform.scale = glm::vec3{ 3.0f };
    m_GameObjects.push_back(std::move(smoothVase));

    std::shared_ptr<Model> flat = Model::CreateModel(
        m_Device,
        "C:\\dev\\VkTest\\VkTest\\src\\Resource\\models\\flat_vase.obj");
    auto flatVase = GameObject::CreateGameObject();
    flatVase.model = flat;
    flatVase.transform.translation = { 0.5f, 0.0f, 2.5f };
    flatVase.transform.scale = glm::vec3{ 3.0f };
    m_GameObjects.push_back(std::move(flatVase));
}

//void Sandbox::Sierpinski(
//        std::vector<Model::Vertex>& vertices, 
//        int depth,
//        glm::vec2 left, 
//        glm::vec2 right, 
//        glm::vec2 top) {
//    if (depth <= 0) {
//        vertices.push_back({ top, 0.0f });
//        vertices.push_back({ right });
//        vertices.push_back({ left });
//    }
//    else {
//        auto leftTop = 0.5f * (left + top);
//        auto rightTop = 0.5f * (right + top);
//        auto leftRight = 0.5f * (left + right);
//        Sierpinski(vertices, depth - 1, left, leftRight, leftTop);
//        Sierpinski(vertices, depth - 1, leftRight, right, rightTop);
//        Sierpinski(vertices, depth - 1, leftTop, rightTop, top);
//    }
//}