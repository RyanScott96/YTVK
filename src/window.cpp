#include "window.hpp"
#include <stdexcept>

namespace YTVK
{
    // Set window params then call GLFW init
    Window::Window(int width, int height, std::string name) : width{width}, height{height}, name{name}
    {
        initWindow();
    }

    // Cleanup GLFW window
    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    };

    // Check if window event close triggered
    bool Window::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    bool Window::wasWindowResized()
    {
        return frameBufferResized;
    }

    void Window::resetWindowResized()
    {
        frameBufferResized = false;
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    VkExtent2D Window::getExtent()
    {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    void Window::initWindow()
    {
        // Initialize GLFW
        glfwInit();

        // "Suggest" no OpenGL and no window resize
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // Create GLFW window
        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
    }

    void Window::frameBufferResizedCallback(GLFWwindow *glfwWindow, int width, int height)
    {
        auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
        window->frameBufferResized = true;
        window->width = width;
        window->height = height;
    }
}