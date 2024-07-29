#include "first_app.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>

// libs
#define GLM_FORCE_RADIANS // forces in radians and not degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan uses 0 to 1, openGL uses 1 to 1
#include <glm/glm.hpp>

namespace lve {

	struct SimplePushConstantData {
		// this is temporary and will be restructured
		glm::vec2 offset;
		alignas(16) glm::vec3 color;

	}; // SimplePushConstantData

	FirstApp::FirstApp() {
	    loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
		
	} // FirstApp

	// this is a check to see if the user has closed the window
	void FirstApp::run() {

		std::cout << "maxPushConstantSize: " << lveDevice.properties.limits.maxPushConstantsSize << "\n";

		while (!lveWindow.shouldClose()) { // the condition checks if they have noc closed it
			glfwPollEvents(); // a window processing events call
			drawFrame();

		} // while

		vkDeviceWaitIdle(lveDevice.device());

	} // run

	void FirstApp::loadModels() {
		/*
		std::vector<LveModel::Vertex> vertices{
			{ {0.0f, -0.5f }, {1.0f, 0.0f, 0.0f } }, // red
			{{ 0.5f, 0.5f  }, {0.0f, 1.0f, 0.0f } }, // blue
			{{ -0.5f, 0.5f }, {0.0f, 0.0f, 1.0f } }, // green

		}; // vertices

		lveModel = std::make_unique<LveModel>(lveDevice, vertices);
		*/

		std::vector<LveModel::Vertex> vertices;
		int depth = 5; // Adjust depth as needed

		// Initial colors for the vertices of the main triangle
		glm::vec3 colorA = { 1.0f, 0.0f, 0.0f }; // red
		glm::vec3 colorB = { 0.0f, 1.0f, 0.0f }; // green
		glm::vec3 colorC = { 0.0f, 0.0f, 1.0f }; // blue

		generateSierpinskiVertices(vertices,
			glm::vec2(0.0f, -0.5f),
			glm::vec2(0.5f, 0.5f),
			glm::vec2(-0.5f, 0.5f),
			colorA, colorB, colorC,
			depth);

		lveModel = std::make_unique<LveModel>(lveDevice, vertices);

	} // loadModels

	void FirstApp::createPipelineLayout() {

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

	void FirstApp::createPipeline() {

		assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		// use the lveSwapChain width and height as the device chain may not match the windows
		//auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain->width(), lveSwapChain->height());
		// Ex: A super display such as the Apple Retina Display the screen coordinate is smaller than number of pixel the window contains

		PipelineConfigInfo pipelineConfig{};

		//PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = lveSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\Push Constants\\simple_shader.vert.spv",
			"C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\Push Constants\\simple_shader.frag.spv",
			pipelineConfig);

	}// createPipeline

	void FirstApp::recreateSwapChain() {
		auto extent = lveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = lveWindow.getExtent();
			glfwWaitEvents();

		} // while

		vkDeviceWaitIdle(lveDevice.device());
		lveSwapChain = nullptr;

		if (lveSwapChain == nullptr) {
			lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);

		} else {
			lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, std::move(lveSwapChain));
			if (lveSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();

			} // if

		} // else

		// if the render pass are compatiable, then do nothing 
		createPipeline();

	} // recreateSwapChain

	void FirstApp::createCommandBuffers() {
		commandBuffers.resize(lveSwapChain->imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lveDevice.getCommandPool();  
		// an application may need to delete and create command buffers frequently so to reduce the cost of resource creation Vulkan has us allocate and free command buffers from command pools
		// this way the most expensive stuff of requiring memory can be done once and be reused as command buffers are created and destroyed
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");

		} // if

	} // createCommandBuffers

	void FirstApp::recordCommandBuffer(int imageIndex) {

		static int frame = 0;
		frame = (frame + 1) % 1000;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");

		} // if

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = lveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f }; // remeber index 0 is color and 1 is depth stencil
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// The vk subpass contents arguement signals that consequent renderpass commands will be directly embedded in the primary 
		// command buffer itself and no secondary buffers will be used

		// the alternative is to use vk subpass contents secondary command buffers, signalling renderpass commands will be executed from secondary command buffers
		// this means no mixing allowed, we cannot have a renderpass that uses both inline commands and secondary command buffers at the same time

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, lveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		lvePipeline->bind(commandBuffers[imageIndex]);
		lveModel->bind(commandBuffers[imageIndex]);

		for (int j = 0; j < 4; j++) {
			SimplePushConstantData push{};
			push.offset = { -0.5f + frame * 0.002f, -0.4f + j * 0.25f };
			push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

			vkCmdPushConstants(
				commandBuffers[imageIndex],
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push

			); // vkCmdPushConstants

			lveModel->draw(commandBuffers[imageIndex]);

		} // for

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin record command buffer!");

		} // if

	} // recordCommandBuffer

	void FirstApp::freeCommandBuffers() {
		vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();

	} // freeCommandBuffers

	void FirstApp::generateSierpinskiVertices(
		std::vector<LveModel::Vertex>& vertices,
		glm::vec2 a, glm::vec2 b, glm::vec2 c,
		glm::vec3 colorA, glm::vec3 colorB, glm::vec3 colorC,
		int depth) {

		if (depth == 0) {
			// Add the vertices of the triangle to the vector
			vertices.push_back({ a, colorA });
			vertices.push_back({ b, colorB });
			vertices.push_back({ c, colorC });
		} else {
			// Calculate the midpoints of each side of the triangle
			glm::vec2 ab = (a + b) / 2.0f;
			glm::vec2 bc = (b + c) / 2.0f;
			glm::vec2 ca = (c + a) / 2.0f;

			// Interpolate colors for the midpoints
			glm::vec3 colorAB = (colorA + colorB) / 2.0f;
			glm::vec3 colorBC = (colorB + colorC) / 2.0f;
			glm::vec3 colorCA = (colorC + colorA) / 2.0f;

			// Recursively generate the vertices for the three smaller triangles
			generateSierpinskiVertices(vertices, a, ab, ca, colorA, colorAB, colorCA, depth - 1);
			generateSierpinskiVertices(vertices, ab, b, bc, colorAB, colorB, colorBC, depth - 1);
			generateSierpinskiVertices(vertices, ca, bc, c, colorCA, colorBC, colorC, depth - 1);

		} // else

	} // generateSierpinskiVertices

	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = lveSwapChain->acquireNextImage(&imageIndex); // handles all the synchronization surronding double and triple buffering 

		// VK_ERROR_OUT_OF_DATE_KHR: A surface that changed in such a way that is no longer compatible with the swap chain
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;

		} // if 

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to aquire the next swapchain image");

		} // if

		recordCommandBuffer(imageIndex);
		result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		// VK_SUBOPTIMAL_KHR: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
			lveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;

		} // if 

		if (result != VK_SUCCESS){
			throw std::runtime_error("failed to aquire the submit command buffers");

		} // if 

	} // drawFrame

	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);

	} // ~FirstApp

} // namespace lve