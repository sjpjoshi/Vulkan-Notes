#pragma once

#include "lve_window.hpp"
#include "lve_pipline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"

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

        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
        //void generateSierpinskiVertices(std::vector<glm::vec2>& vertices, glm::vec2 a, glm::vec2 b, glm::vec2 c, int depth);
        void generateSierpinskiVertices(
            std::vector<LveModel::Vertex>& vertices,
            glm::vec2 a, glm::vec2 b, glm::vec2 c,
            glm::vec3 colorA, glm::vec3 colorB, glm::vec3 colorC,
            int depth);

        LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
        LveDevice lveDevice{ lveWindow };
        LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };

        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout; 
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<LveModel> lveModel;

        // Use the actual paths to your SPIR-V shader files here
        /*
          LvePipeline lvePipeline{
            lveDevice,
            "C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\GraphicsPipelineOverview\\simple_shader.vert.spv",
            "C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\GraphicsPipelineOverview\\simple_shader.frag.spv",
            LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)

        }; // lvePipeline      
        */
  

    }; // FirstApp

} // namespace lve
