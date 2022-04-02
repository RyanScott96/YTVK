#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "frame_info.hpp"

#include <memory>
#include <vector>

namespace YTVK
{

    class RenderSystem
    {
    public:
        RenderSystem(Device &, VkRenderPass, VkDescriptorSetLayout);
        ~RenderSystem();
        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;

        void renderGameObjects(FrameInfo &, std::vector<GameObject> &);

    private:
        void createPipelineLayout(VkDescriptorSetLayout);
        void createPipeline(VkRenderPass);

        Device &device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
};