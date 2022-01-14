#pragma once

#include "Window.h"

#include <string>
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR        m_Capabilities;
    std::vector<VkSurfaceFormatKHR> m_Formats;
    std::vector<VkPresentModeKHR>   m_PresentModes;
};

struct QueueFamilyIndices {
public:
    bool IsComplete() { return m_GraphicsFamilyHasValue && m_PresentFamilyHasValue; }
public:
    uint32_t    m_GraphicsFamily;
    uint32_t    m_PresentFamily;
    bool        m_GraphicsFamilyHasValue = false;
    bool        m_PresentFamilyHasValue = false;
};

class Device {
public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
    Device(Window &window);
    ~Device();

    Device(const Device &) = delete;
    void operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;
public:
    VkCommandPool GetCommandPool() { return m_CommandPool; }
    VkDevice GetDevice() { return m_Device_; }
    VkSurfaceKHR Surface() { return m_Surface_; }
    VkQueue GraphicsQueue() { return m_GraphicsQueue_; }
    VkQueue PresentQueue() { return m_PresentQueue_; }
    
    SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
    VkFormat FindSupportedFormat(
            const std::vector<VkFormat> &candidates, 
            VkImageTiling tiling,
            VkFormatFeatureFlags features);

    void CreateBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer &buffer,
            VkDeviceMemory &bufferMemory);

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
    
    void CreateImageWithInfo(
        const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory);
public:
    VkPhysicalDeviceProperties properties;
private:
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();
    
    bool IsDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char*> GetRequiredExtensions();
    bool CheckValidationLayerSupport();
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void HasGflwRequiredInstanceExtensions();
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
private:
    VkInstance                      m_Instance;
    VkDebugUtilsMessengerEXT        m_DebugMessenger;
    VkPhysicalDevice                m_PhysicalDevice = VK_NULL_HANDLE;
    Window&                         m_Window;
    VkCommandPool                   m_CommandPool;
                                    
    VkDevice                        m_Device_;
    VkSurfaceKHR                    m_Surface_;
    VkQueue                         m_GraphicsQueue_;
    VkQueue                         m_PresentQueue_;
    
    const std::vector<const char *> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};