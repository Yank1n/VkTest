#pragma once

#include <Core/Device.h>
#include <Core/SwapChain.h>
#include <Core/Window.h>

#include <memory>
#include <vector>

class Renderer {
public:
    Renderer(Window& window, Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
public:
    VkCommandBuffer BeginFrame();
    void EndFrame();
    void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
    bool IsFrameInProgress() const;
    VkCommandBuffer GetCurrentCommandBuffer() const;
    VkRenderPass GetSwapChainRenderPass() const;
    uint32_t GetFrameIndex() const;
    float GetAspectRatio() const;
private:
    void CreateCommandBuffers();
    void RecreateSwapChain();
    void FreeCommandBuffer();
private:
    Window&						 m_Window;
    Device&						 m_Device;
    std::unique_ptr<SwapChain>	 m_pSwapChain;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    uint32_t					 m_CurrentImageIndex;
    uint32_t					 m_CurrentFrameIndex = 0;
    bool						 m_IsFrameStarted = false;
};