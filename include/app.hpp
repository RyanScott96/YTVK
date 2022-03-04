#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "game_object.hpp"

#include <memory>
#include <vector>

namespace YTVK {

    struct SimplePushConstantData {
        glm::mat2 transform {1.0f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    class App {
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();
        App(const App&) = delete;
        App &operator=(const App&) = delete;

        void run();

        private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void loadGameObjects();
        void drawFrame();
        void recreateSwapchain();
        void recordCommandBuffer(int);
        void renderGameObjects(VkCommandBuffer commandBuffer);


        Window window{WIDTH, HEIGHT, "Window!"};
        Device device{window};
        std::unique_ptr<SwapChain> swapchain;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<GameObject> gameObjects;
    };
};