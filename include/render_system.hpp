#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"

#include <memory>
#include <vector>

namespace YTVK
{

    class RenderSystem
    {
    public:
        RenderSystem(Device &, VkRenderPass);
        ~RenderSystem();
        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer, std::vector<GameObject> &);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass);

        Device &device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
};