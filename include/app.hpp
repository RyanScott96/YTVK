#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"
#include "game_object.hpp"

#include <memory>
#include <vector>

namespace YTVK
{

    class App
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();
        App(const App &) = delete;
        App &operator=(const App &) = delete;

        void run();

    private:
        void loadGameObjects();

        Window window;
        Device device;
        Renderer renderer;
        std::unique_ptr<DescriptorPool> globalPool;
        std::vector<GameObject> gameObjects;
    };
};