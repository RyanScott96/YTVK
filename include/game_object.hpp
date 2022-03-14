#pragma once

#include "model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace YTVK
{
    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class GameObject
    {
    public:
        GameObject(const GameObject &) = delete;
        GameObject &operator=(const GameObject &) = delete;
        GameObject(GameObject &&) = default;
        GameObject &operator=(GameObject &&) = default;

        using id_t = unsigned int;
        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        TransformComponent transform{};

        id_t getId() { return id; }

        static GameObject createGameObject()
        {
            static id_t current_id = 0;
            return GameObject(current_id++);
        }

    private:
        GameObject(id_t obj_id) : id(obj_id){};
        id_t id;
    };
};