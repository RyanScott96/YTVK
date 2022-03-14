#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace YTVK
{
    class Model
    {
    public:
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAtributeDescriptions();

            bool operator==(const Vertex &) const;
        };

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string &);
        };

        Model(Device &, const Model::Builder &);
        ~Model();
        Model(const Model &) = delete;
        Model &operator=(const Model &) = delete;

        void bind(VkCommandBuffer);
        void draw(VkCommandBuffer);

        static std::unique_ptr<Model> createModelFromFile(Device&, const std::string &);

    private:
        Device &device;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        bool hasIndexBuffer;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;

        void createVertexBuffers(const std::vector<Vertex> &);
        void createIndexBuffers(const std::vector<uint32_t> &);
    };
}