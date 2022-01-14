#include "Window.h"

#include <stdexcept>

Window::Window(const uint32_t width, const uint32_t height, const std::string& title)
    : m_Width{ width }, m_Height{ height }, m_Title{ title } {
    Init();
}

Window::~Window() {
    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void Window::FramebufferResizedCallback(GLFWwindow* window, int width, int height){
    auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    win->m_FrameBufferResized = true;
    win->m_Width = width;
    win->m_Height = height;
}

void Window::Init() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_Window = glfwCreateWindow(static_cast<int>(m_Width), 
                                static_cast<int>(m_Height), 
                                m_Title.c_str(), 
                                nullptr, 
                                nullptr);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, FramebufferResizedCallback);
}

int Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

VkExtent2D Window::GetExtend() const {
    return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) };
}

bool Window::WasResized(){
    return m_FrameBufferResized;
}

void Window::ResetResizedFlag(){
    m_FrameBufferResized = false;
}

void Window::CreateWindowSurface(VkInstance inst, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(inst, m_Window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

GLFWwindow* Window::GetNativeWindow() const {
    return m_Window;
}
