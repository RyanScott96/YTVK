#pragma once

#include "model.hpp"

#include <memory>

namespace YTVK {
    struct Transform2DComponent {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        glm::mat2 mat2() { 
            const float sin = glm::sin(rotation);
            const float cos = glm::cos(rotation);
            glm::mat2 rotationMat{{cos, sin}, {-sin, cos}};

            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
            return rotationMat * scaleMat;
        }
    };

    class GameObject {
        public:
        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;

        using id_t = unsigned int;
        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        Transform2DComponent transform2D{};

        id_t getId() { return id; }

        static GameObject createGameObject() {
            static id_t current_id = 0;
            return GameObject(current_id++);
        }

        private:
        GameObject(id_t obj_id) : id(obj_id){};
        id_t id;
    };
};