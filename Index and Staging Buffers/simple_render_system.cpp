#include "simple_render_system.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>
#include <chrono>

// libs
#define GLM_FORCE_RADIANS // forces in radians and not degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan uses 0 to 1, openGL uses 1 to 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

	struct SimplePushConstantData {
		// this is temporary and will be restructured
		glm::mat4 transform{ 1.f };
		alignas(16) glm::vec3 color;

	}; // SimplePushConstantData

	SimpleRenderSystem::SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass) : lveDevice{device} {
		createPipelineLayout();
		createPipeline(renderPass);

	} // SimpleRenderSystem


	void SimpleRenderSystem::createPipelineLayout() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0; // mainly used for if you are using separate ranges for the vertex and fragment shaders
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		/// a pipeline set layout to send data other than our vertex data to our vertex and fragment shaders
		// this can include textures and uniform buffer objects which will be covered later
		pipelineLayoutInfo.pSetLayouts = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;

		// push constants are a way to send efficiently a very small amount of data through to our shader programs
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");

		} // if

	} // createPipelineLayout

	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {

		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};

		LvePipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\Index and Staging Buffers\\simple_shader.vert.spv",
			"C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\Index and Staging Buffers\\simple_shader.frag.spv",
			pipelineConfig);

	}// createPipeline

	void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject>& gameObjects, const LveCamera& camera) {
		lvePipeline->bind(commandBuffer);
		auto projectionView = camera.getProjection() * camera.getView(); // every rendered object will used the same projection and view matrix, so this way we can avoid doing the calculation for each iterated view function

		for (auto& obj : gameObjects) {
			SimplePushConstantData push{};
			push.color = obj.color;
			// this is temp solution
			push.transform = projectionView * obj.transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push

			); // vkCmdPushConstants

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);

		} // for

	} // renderGameObjects

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);

	} // ~SimpleRenderSystem

} // namespace lve