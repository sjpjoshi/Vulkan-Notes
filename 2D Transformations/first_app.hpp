#pragma once

#include "lve_window.hpp"
#include "lve_pipline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
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
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void freeCommandBuffers();
        void renderGameObjects(VkCommandBuffer commandBuffer);

        void generateSierpinskiVertices(
            std::vector<LveModel::Vertex>& vertices,
            glm::vec2 a, glm::vec2 b, glm::vec2 c,
            glm::vec3 colorA, glm::vec3 colorB, glm::vec3 colorC,
            int depth);

        LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
        LveDevice lveDevice{ lveWindow };
        // using a unique ptr rather than a stack allocated variable we can easily recreate the width and height by simply recreating a new object
        // however using pointers comes at a small performance cost 
        std::unique_ptr<LveSwapChain> lveSwapChain;

        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout; 
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<LveModel> lveModel;
        std::vector<LveGameObject> gameObjects;



    }; // FirstApp

} // namespace lve
