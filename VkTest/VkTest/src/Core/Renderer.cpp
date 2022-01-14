#include "Renderer.h"

#include <stdexcept>
#include <array>
#include <memory>

Renderer::Renderer(Window& window, Device& device) 
    : m_Window{ window }, m_Device{device} {
    RecreateSwapChain();
    CreateCommandBuffers();
}

Renderer::~Renderer() {
    FreeCommandBuffer();
}

VkCommandBuffer Renderer::BeginFrame() {
    auto result = m_pSwapChain->AcquireNextImage(&m_CurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to acqure swap chain image!");
    }

    m_IsFrameStarted = true;
    auto commandBuffer = GetCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void Renderer::EndFrame() {
    auto commandBuffer = GetCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

    auto result = m_pSwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasResized()) {
        m_Window.ResetResizedFlag();
        RecreateSwapChain();
        return;
    }
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_IsFrameStarted = false;
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_pSwapChain->GetRenderPass();
    renderPassInfo.framebuffer = m_pSwapChain->GetFrameBuffer(static_cast<int>(m_CurrentImageIndex));

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_pSwapChain->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.01f, 0.1f, 0.1f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_pSwapChain->GetSwapChainExtent().width);
    viewport.height = static_cast<float>(m_pSwapChain->GetSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, m_pSwapChain->GetSwapChainExtent() };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

bool Renderer::IsFrameInProgress() const {
    return m_IsFrameStarted;
}

VkCommandBuffer Renderer::GetCurrentCommandBuffer() const {
    return m_CommandBuffers[m_CurrentFrameIndex];
}

VkRenderPass Renderer::GetSwapChainRenderPass() const {
    return m_pSwapChain->GetRenderPass();
}

uint32_t Renderer::GetFrameIndex() const {
    return m_CurrentFrameIndex;
}

float Renderer::GetAspectRatio() const {
    return m_pSwapChain->ExtentAspectRatio();
}

void Renderer::CreateCommandBuffers() {
    m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_Device.GetCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    if (vkAllocateCommandBuffers(m_Device.GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate commad buffers!");
    }
}

void Renderer::RecreateSwapChain() {
    auto extent = m_Window.GetExtend();
    while (extent.width == 0 || extent.height == 0) {
        extent = m_Window.GetExtend();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_Device.GetDevice());
    if (m_pSwapChain == nullptr)
        m_pSwapChain = std::make_unique<SwapChain>(m_Device, extent);
    else {
        std::shared_ptr<SwapChain> oldSwapChain = std::move(m_pSwapChain);
        m_pSwapChain = std::make_unique<SwapChain>(m_Device, extent, oldSwapChain);

        if (!oldSwapChain->CompareSwapFormat(*m_pSwapChain.get())) {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
    // TODO: Pipiline
}

void Renderer::FreeCommandBuffer() {
    vkFreeCommandBuffers(
        m_Device.GetDevice(),
        m_Device.GetCommandPool(),
        static_cast<uint32_t>(m_CommandBuffers.size()),
        m_CommandBuffers.data());
    m_CommandBuffers.clear();
}
