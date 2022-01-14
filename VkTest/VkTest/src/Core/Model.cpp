#include "Model.h"
#include <Core/Utils.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>

namespace std {
    template <>
    struct hash<Model::Vertex> {
        size_t operator()(Model::Vertex const& vertex) const {
            size_t seed{};
            hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() {
    return { {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX} };
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttribDescriptions(){
    return { 
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)}, 
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
        {3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)}
    };
}

bool Model::Vertex::operator==(const Vertex& other) const {
    return position == other.position && 
           color == other.color && 
           normal == other.normal && 
           uv == other.uv;
}

Model::Model(Device& device, const Model::Builder& builder)
    : m_Device{device} {
    CreateVertexBuffer(builder.vertices);
    CreateIndexBuffer(builder.indices);
}

Model::~Model() {
    vkDestroyBuffer(m_Device.GetDevice(), m_VertexBuffer, nullptr);
    vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);

    if (m_HasIndexBuffer) {
        vkDestroyBuffer(m_Device.GetDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), m_IndexBufferMemory, nullptr);
    }
}

void Model::Bind(VkCommandBuffer& cmdBuffer) {
    VkBuffer buffers[]{ m_VertexBuffer };
    VkDeviceSize offsets[]{ 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, buffers, offsets);

    if (m_HasIndexBuffer) {
        vkCmdBindIndexBuffer(cmdBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
}

void Model::Draw(VkCommandBuffer& cmdBuffer) {
    if (m_HasIndexBuffer) {
        vkCmdDrawIndexed(cmdBuffer, m_IndexCount, 1, 0, 0, 0);
    }
    else {
        vkCmdDraw(cmdBuffer, m_VertexCount, 1, 0, 0);
    }
}

std::unique_ptr<Model> Model::CreateModel(Device& device, const std::string& filepath) {
    Builder builder{};
    builder.LoadModel(filepath);

    return std::make_unique<Model>(device, builder);
}

void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    m_VertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    m_Device.CreateBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

    m_Device.CreateBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_VertexBuffer,
        m_VertexBufferMemory);

    m_Device.CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);
    vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
}

void Model::CreateIndexBuffer(const std::vector<uint32_t>& indices) {
    m_IndexCount = static_cast<uint32_t>(indices.size());
    m_HasIndexBuffer = m_IndexCount > 0;

    if (!m_HasIndexBuffer) {
        return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    m_Device.CreateBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

    m_Device.CreateBuffer(
        bufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_IndexBuffer,
        m_IndexBufferMemory);

    m_Device.CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);
    vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
}

void Model::Builder::LoadModel(const std::string& filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2]
                };
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}
