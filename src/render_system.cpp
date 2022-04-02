#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace YTVK
{

    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };

    RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    RenderSystem::~RenderSystem()
    {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(
                device.device(),
                &pipelineLayoutInfo,
                nullptr,
                &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void RenderSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before layout");

        PipelineConfigInfo pipelineConfig = {};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device,
            pipelineConfig,
            "shaders/simple.vert.spv",
            "shaders/simple.frag.spv");
    }

    void RenderSystem::renderGameObjects(FrameInfo &frameInfo, std::vector<GameObject> &gameObjects)
    {
        pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0, 1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        for (auto &object : gameObjects)
        {
            SimplePushConstantData push{};
            push.modelMatrix = object.transform.mat4();
            push.normalMatrix = object.transform.normalMatrix();

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            object.model->bind(frameInfo.commandBuffer);
            object.model->draw(frameInfo.commandBuffer);
        }
    }
}