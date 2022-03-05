#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace YTVK
{

    class Window
    {
    public:
        Window(int width, int height, std::string name);
        ~Window();
        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        bool shouldClose();
        bool wasWindowResized();
        void resetWindowResized();
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        VkExtent2D getExtent();
        GLFWwindow* getGLFWwindow() const;

    private:
        GLFWwindow *window;
        int width;
        int height;
        bool frameBufferResized = false;
        std::string name;

        void initWindow();

        static void frameBufferResizedCallback(GLFWwindow *, int width, int height);
    };
};