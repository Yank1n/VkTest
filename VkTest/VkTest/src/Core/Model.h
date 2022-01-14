#pragma once

#include <Core/Device.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Model{
public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttribDescriptions();

        bool operator==(const Vertex& other) const;
    };

    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        void LoadModel(const std::string& filepath);
    };
public:
    Model(Device& device, const Model::Builder& builder);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) = delete;
    Model& operator=(Model&&) = delete;
public:
    void Bind(VkCommandBuffer& cmdBuffer);
    void Draw(VkCommandBuffer& cmdBuffer);
    static std::unique_ptr<Model> CreateModel(Device& device, const std::string& filepath);
private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);
private:
    Device&			m_Device;

    VkBuffer		m_VertexBuffer;
    VkDeviceMemory	m_VertexBufferMemory;
    uint32_t		m_VertexCount;

    VkBuffer		m_IndexBuffer;
    VkDeviceMemory	m_IndexBufferMemory;
    uint32_t		m_IndexCount;

    bool m_HasIndexBuffer = false;
};