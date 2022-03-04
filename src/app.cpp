#include "app.hpp"
#include <stdexcept>

namespace YTVK{
    App::App(){
        loadGameObjects();
        createPipelineLayout();
        recreateSwapchain();
        createCommandBuffers();
    }

    App::~App() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void App::run() {
        while(!window.shouldClose()) {
            // Check GLFW for events
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(device.device());
    };

    void App::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(
            device.device(),
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void App::createPipeline() {
        assert(swapchain != nullptr && "Cannot create pipeline before swapchain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before layout");

        PipelineConfigInfo pipelineConfig = {};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = swapchain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device,
            pipelineConfig,
            "shaders/simple.vert.spv",
            "shaders/simple.frag.spv"
        );
    }

    void App::createCommandBuffers() {
        commandBuffers.resize(swapchain->imageCount());
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = device.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    };

    void App::freeCommandBuffers() {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void App::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain->getRenderPass();
        renderPassInfo.framebuffer = swapchain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{{0, 0}, swapchain->getSwapChainExtent()};

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        renderGameObjects(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void App::recreateSwapchain() {
        auto extent = window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device());

        if (swapchain == nullptr) {
            swapchain = std::make_unique<SwapChain>(device, extent);
        }
        else {
            swapchain = std::make_unique<SwapChain>(device, extent, std::move(swapchain));
            if (swapchain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // Potentially reuse pipeline if renderpass is compatible
        createPipeline();
    }

    void App::loadGameObjects() {
        std::vector<Model::Vertex> vertices{};
        vertices = std::vector<Model::Vertex>{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
        auto model = std::make_shared<Model>(device, vertices);

        auto triangle = GameObject::createGameObject();
        triangle.model = model;
        triangle.color = {0.1f, 0.8f, 0.1f};
        triangle.transform2D.translation.x = 0.2f;
        triangle.transform2D.scale = {2.0f, 0.5f};
        triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void App::drawFrame() {
        uint32_t imageIndex;
        auto result = swapchain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        recordCommandBuffer(imageIndex);
        result = swapchain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR
            || result == VK_SUBOPTIMAL_KHR
            || window.wasWindowResized()) {
                window.resetWindowResized();
                recreateSwapchain();
                return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    };

    void App::renderGameObjects(VkCommandBuffer commandBuffer) {
        pipeline->bind(commandBuffer);
        for (auto& object : gameObjects) {
            object.transform2D.rotation = glm::mod(object.transform2D.rotation + 0.001f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = object.transform2D.translation;
            push.color = object.color;
            push.transform = object.transform2D.mat2();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            object.model->bind(commandBuffer);
            object.model->draw(commandBuffer);
        }
    }
}