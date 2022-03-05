#include "renderer.hpp"
#include <stdexcept>
#include <cassert>

namespace YTVK
{
    Renderer::Renderer(Window &window, Device &device) : window{window}, device{device}, isFrameStarted{false}, currentFrameIndex{0}
    {
        recreateSwapchain();
        createCommandBuffers();
    }

    Renderer::~Renderer()
    {
        freeCommandBuffers();
    }

    void Renderer::createCommandBuffers()
    {
        commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = device.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void Renderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void Renderer::recreateSwapchain()
    {
        auto extent = window.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device());

        if (swapchain == nullptr)
        {
            swapchain = std::make_unique<SwapChain>(device, extent);
        }
        else
        {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapchain);
            swapchain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*swapchain.get()))
            {
                throw std::runtime_error("Swap chain image or depth format has changed");
            }
        }

        // TODO: something
    }

    VkCommandBuffer Renderer::beginFrame()
    {
        assert(!isFrameStarted && "Cannot start frame while frame is already in progress");

        auto result = swapchain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapchain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        return commandBuffer;
    }

    void Renderer::endFrame()
    {
        assert(isFrameStarted && "Cannot end frame unless frame is in progress");

        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = swapchain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
        {
            window.resetWindowResized();
            recreateSwapchain();
        }

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    bool Renderer::isFrameInProgress() const
    {
        return isFrameStarted;
    }

    VkCommandBuffer Renderer::getCurrentCommandBuffer() const
    {
        assert(isFrameStarted && "Cannot get command buffer when frame not in progres");
        return commandBuffers[currentFrameIndex];
    }

    int Renderer::getCurrentFrameIndex() const
    {
        assert(isFrameStarted && "Cannot get frame when frame not in progres");
        return currentFrameIndex;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Cannot begin swapchain render pass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain->getRenderPass();
        renderPassInfo.framebuffer = swapchain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{{0, 0}, swapchain->getSwapChainExtent()};

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Cannot end swapchain render pass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on a different frame");
        vkCmdEndRenderPass(commandBuffer);
    }

    VkRenderPass Renderer::getSwapChainRenderPass() const
    {
        return swapchain->getRenderPass();
    }
}