#pragma once

#include <Core/Device.h>
#include <Core/Window.h>
#include <Core/GameObject.h>
#include <Core/Renderer.h>

#include <memory>
#include <vector>

class Sandbox{
public:
    static constexpr uint32_t s_Width = 800;
    static constexpr uint32_t s_Height = 600;
public:
    Sandbox();
    ~Sandbox();

    Sandbox(const Sandbox&) = delete;
    Sandbox& operator=(const Sandbox&) = delete;

    Sandbox(Sandbox&&) = delete;
    Sandbox& operator=(Sandbox&&) = delete;
public:
    void Run();
private:
    void LoadGameObjects();
    void Sierpinski(
        std::vector<Model::Vertex>& vertices, 
        int depth, 
        glm::vec2 left,
        glm::vec2 right,
        glm::vec2 top);
private:
    Window						m_Win{ s_Width, s_Height };
    Device						m_Device{ m_Win };
    Renderer					m_Renderer{ m_Win, m_Device };
    std::vector<GameObject>		m_GameObjects;
};