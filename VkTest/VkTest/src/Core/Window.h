#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window{
public:
    Window(const uint32_t width, const uint32_t height, const std::string& title = "Octo");
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
public:
    int ShouldClose() const;
    VkExtent2D GetExtend() const;
    bool WasResized();
    void ResetResizedFlag();
    void CreateWindowSurface(VkInstance inst, VkSurfaceKHR* surface);
    GLFWwindow* GetNativeWindow() const;
private:
    static void FramebufferResizedCallback(GLFWwindow* window, int width, int height);
    void Init();
private:
    uint32_t	m_Width;
    uint32_t	m_Height;
    bool		m_FrameBufferResized;
    std::string m_Title;
    GLFWwindow* m_Window;
};