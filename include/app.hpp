#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "game_object.hpp"

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
        void loadGameObjects();
        void renderGameObjects(VkCommandBuffer commandBuffer);


        Window window{WIDTH, HEIGHT, "Window!"};
        Device device{window};
        Renderer renderer{window, device};
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<GameObject> gameObjects;
    };
};