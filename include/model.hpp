#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <vector>

namespace YTVK
{
    class Model
    {
    public:
        struct Vertex
        {
            glm::vec2 position;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAtributeDescriptions();
        };

        Model(Device &, const std::vector<Vertex> &);
        ~Model();
        Model(const Model &) = delete;
        Model &operator=(const Model &) = delete;

        void bind(VkCommandBuffer);
        void draw(VkCommandBuffer);

    private:
        Device &device;
        VkBuffer vertexBuffer;
        VkDeviceMemory bufferMemory;
        uint32_t vertexCount;

        void createVertexBuffers(const std::vector<Vertex> &vertices);
    };
};