#pragma once
#include "device.hpp"
#include <string>
#include <vector>

namespace YTVK {
    struct PipelineConfigInfo {
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline {
        public:
        Pipeline(   
            Device &device,
            const PipelineConfigInfo& configInfo,
            const std::string &vertFilePath,
            const std::string &fragFilePath);

        ~Pipeline();
        Pipeline(const Pipeline&) = delete;
        Pipeline &operator=(const Pipeline&) = delete;

        static void defaultPipelineConfigInfo(PipelineConfigInfo&);

        void bind(VkCommandBuffer commandBuffer);

        private:
        Device &device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        
        static std::vector<char> readFile(const std::string &filePath);

        void createGraphicsPipeline(
            const std::string &vertFilePath,
            const std::string &fragFilePath,
            const PipelineConfigInfo &configInfo);

        void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);
    };
};