#pragma once

#include <Core/Device.h>
#include <Core/Pipeline.h>
#include <Core/GameObject.h>
#include <Core/Camera.h>

#include <memory>
#include <vector>

class RenderSystem {
public:
    RenderSystem(Device& device, VkRenderPass renderPass);
    ~RenderSystem();

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    RenderSystem(RenderSystem&&) = delete;
    RenderSystem& operator=(RenderSystem&&) = delete;
public:
    void RenderGameObjects(
        VkCommandBuffer commandBuffer, 
        std::vector<GameObject>& gameObjects,
        Camera& camera
    );
private:
    void CreatePipeline(VkRenderPass renderPass);
    void CreatePipelineLayout();
private:
    Device&						m_Device;
    std::unique_ptr<Pipeline>	m_pPipeline;
    VkPipelineLayout			m_PipelineLayot;
};