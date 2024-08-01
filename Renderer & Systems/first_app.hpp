#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"
#include "lve_game_object.hpp"

// std
#include <memory>
#include <vector>

namespace lve {

    class FirstApp {
    public:
        int static constexpr WIDTH = 800;
        int static constexpr HEIGHT = 600;
        void run();

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp& operator=(const FirstApp&) = delete;

    private:

        void loadGameObjects();
        void generateSierpinskiVertices(
            std::vector<LveModel::Vertex>& vertices,
            glm::vec2 a, glm::vec2 b, glm::vec2 c,
            glm::vec3 colorA, glm::vec3 colorB, glm::vec3 colorC,
            int depth);

        LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
        LveDevice lveDevice{ lveWindow };
        LveRenderer lveRenderer{ lveWindow, lveDevice };
        std::unique_ptr<LveModel> lveModel;

        std::vector<LveGameObject> gameObjects;

    }; // FirstApp

} // namespace lve
