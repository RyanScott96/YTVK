#pragma once
#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "model.hpp"
#include <memory>
#include <vector>

namespace YTVK {
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
        void loadModels();
        void drawFrame();
        void recreateSwapchain();
        void recordCommandBuffer(int);


        Window window{WIDTH, HEIGHT, "Window!"};
        Device device{window};
        std::unique_ptr<SwapChain> swapchain;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<Model> model;
    };
};