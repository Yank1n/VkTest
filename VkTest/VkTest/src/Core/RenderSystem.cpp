#include "RenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

struct PushConstantData {
    glm::mat4 transform{ 1.0f };
    glm::mat4 normalMatrix{ 1.0f };
};

RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass) 
    : m_Device{device} {
    CreatePipelineLayout();
    CreatePipeline(renderPass);
}

RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayot, nullptr);
}

void RenderSystem::RenderGameObjects(
    VkCommandBuffer commandBuffer, 
    std::vector<GameObject>& gameObjects, 
    Camera& camera
) {
    m_pPipeline->Bind(commandBuffer);

    auto projectionView = camera.GetProjection() * camera.GetView();

    for (auto& obj : gameObjects) {
        //obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.0005f, glm::two_pi<float>());
        //obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.0001f, glm::two_pi<float>());

        PushConstantData push{};
        auto modelMatrix = obj.transform.mat4();
        push.transform = projectionView * modelMatrix;
        push.normalMatrix = obj.transform.NormalMatrix();

        vkCmdPushConstants(
            commandBuffer,
            m_PipelineLayot,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PushConstantData),
            &push
        );

        obj.model->Bind(commandBuffer);
        obj.model->Draw(commandBuffer);
    }
}

void RenderSystem::CreatePipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig;
    Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayot;
    m_pPipeline = std::make_unique<Pipeline>(
        m_Device,
        "C:/dev/VkTest/VkTest/src/Shaders/spv.vert",
        "C:/dev/VkTest/VkTest/src/Shaders/spv.frag",
        pipelineConfig);
}

void RenderSystem::CreatePipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayot) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}