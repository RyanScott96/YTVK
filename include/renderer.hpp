#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"

#include <memory>
#include <vector>

namespace YTVK
{
    class Renderer
    {
    public:
        Renderer(Window &, Device &);
        ~Renderer();
        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;

        VkCommandBuffer beginFrame();
        void endFrame();
        bool isFrameInProgress() const;

        VkCommandBuffer getCurrentCommandBuffer() const;
        int getCurrentFrameIndex() const;

        float getAspectRation() const;

        void beginSwapChainRenderPass(VkCommandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer);
        VkRenderPass getSwapChainRenderPass() const;

    private:
        Window &window;
        Device &device;

        uint32_t currentImageIndex;
        uint32_t currentFrameIndex;
        bool isFrameStarted;

        std::unique_ptr<SwapChain> swapchain;
        std::vector<VkCommandBuffer> commandBuffers;

        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapchain();
    };
};