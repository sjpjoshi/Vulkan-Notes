#pragma once

#include "lve_pipline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_camera.hpp"

// std
#include <memory>
#include <vector>

namespace lve {

    class SimpleRenderSystem {
    public:

        SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
        ~SimpleRenderSystem();
        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject>& gameObjects, const LveCamera &camera);

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        LveDevice& lveDevice;

        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<LveModel> lveModel;

    }; // SimpleRenderSystem

} // namespace lve
