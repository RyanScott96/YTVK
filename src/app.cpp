#include "app.hpp"
#include "render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
#include <stdexcept>

namespace YTVK
{
    struct GlobalUBO
    {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
    };

    App::App()
    {
        loadGameObjects();
    }

    App::~App() {}

    void App::run()
    {
        Buffer globalUBOBuffer
        {
            device,
            sizeof(GlobalUBO),
            SwapChain::MAX_FRAMES_IN_FLIGHT,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            device.properties.limits.minUniformBufferOffsetAlignment
        };

        globalUBOBuffer.map();

        RenderSystem renderSystem{device, renderer.getSwapChainRenderPass()};
        Camera camera{};

        auto viewerObject = GameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!window.shouldClose())
        {
            // Check GLFW for events
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = renderer.getAspectRation();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = renderer.beginFrame())
            {
                int frameIndex = renderer.getCurrentFrameIndex();
                GlobalUBO ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                globalUBOBuffer.writeToIndex(&ubo, frameIndex);
                globalUBOBuffer.flushIndex(frameIndex);

                renderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    };

    void App::loadGameObjects()
    {
        std::shared_ptr<Model> flatVaseModel = Model::createModelFromFile(device, "models/flat_vase.obj");

        auto flatVaseObject = GameObject::createGameObject();
        flatVaseObject.model = flatVaseModel;
        flatVaseObject.transform.translation = {1.0f, 0.0f, 2.5f};
        flatVaseObject.transform.scale = {0.5f, 0.25f, 0.5f};

        std::shared_ptr<Model> smoothVaseModel = Model::createModelFromFile(device, "models/smooth_vase.obj");
        auto smoothVaseObject = GameObject::createGameObject();
        smoothVaseObject.model = smoothVaseModel;
        smoothVaseObject.transform.translation = {-1.0f, 0.0f, 2.5f};
        smoothVaseObject.transform.scale = {0.5f, 0.5f, 0.25f};

        gameObjects.push_back(std::move(flatVaseObject));
        gameObjects.push_back(std::move(smoothVaseObject));
    }
}