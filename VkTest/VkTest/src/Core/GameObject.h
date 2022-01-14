#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <Core/Model.h>

#include <memory>

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
    glm::vec3 rotation{};

    glm::mat4 mat4();
    glm::mat3 NormalMatrix();
};

// TODO: normal ECS
class GameObject {
public:
    using id_t = size_t;

    static GameObject CreateGameObject() {
        static id_t currentId = 0;
        return GameObject(currentId++);
    }
    
    id_t GetID() const { return m_ID; }

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    GameObject(GameObject&&) = default;
    GameObject& operator=(GameObject&&) = default;
public:
    std::shared_ptr<Model> model{};
    glm::vec3 color{};
    TransformComponent transform{};
private:
    GameObject(id_t id) : m_ID{ id } {}
private:
    id_t m_ID;
};