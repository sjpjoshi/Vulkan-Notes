#include "first_app.hpp"

// std
#include <stdexcept>
#include <array>

namespace lve {

	FirstApp::FirstApp() {
	    loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
		
	} // FirstApp

	// this is a check to see if the user has closed the window
	void FirstApp::run() {
		while (!lveWindow.shouldClose()) { // the condition checks if they have noc closed it
			glfwPollEvents(); // a window processing events call
			drawFrame();

		} // while

		vkDeviceWaitIdle(lveDevice.device());

	} // run

	void FirstApp::loadModels() {
		std::vector<LveModel::Vertex> vertices{ 
			{ {0.0f, -0.5f }}, 
			{{ 0.5f, 0.5f  }}, 
			{{ -0.5f, 0.5f }}, 
		}; // vertices

		lveModel = std::make_unique<LveModel>(lveDevice, vertices);

	} // loadModels

	void FirstApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		/// a pipeline set layout to send data other than our vertex data to our vertex and fragment shaders
		// this can include textures and uniform buffer objects which will be covered later
		pipelineLayoutInfo.pSetLayouts = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;

		// push constants are a way to send efficiently a very small amount of data through to our shader programs
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		} // if

	} // createPipelineLayout

	void FirstApp::createPipeline() {
		// use the lveSwapChain width and height as the device chain may not match the windows
		//auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
		// Ex: A super display such as the Apple Retina Display the screen coordinate is smaller than number of pixel the window contains

		PipelineConfigInfo pipelineConfig{};


		//PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(
			pipelineConfig,
			lveSwapChain.width(),
			lveSwapChain.height());
		pipelineConfig.renderPass = lveSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\GraphicsPipelineOverview\\simple_shader.vert.spv",
			"C:\\Users\\suraj\\OneDrive\\Documents\\Visual Studio Projects\\Vulkan Notes\\GraphicsPipelineOverview\\simple_shader.frag.spv",
			pipelineConfig);
	}

	void FirstApp::createCommandBuffers() {
		commandBuffers.resize(lveSwapChain.imageCount());
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

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");

			} // if

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = lveSwapChain.getRenderPass();
			renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; // remeber index 0 is color and 1 is depth stencil
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			// The vk subpass contents arguement signals that consequent renderpass commands will be directly embedded in the primary 
			// command buffer itself and no secondary buffers will be used

			// the alternative is to use vk subpass contents secondary command buffers, signalling renderpass commands will be executed from secondary command buffers
			// this means no mixing allowed, we cannot have a renderpass that uses both inline commands and secondary command buffers at the same time

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			lvePipeline->bind(commandBuffers[i]);
			//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
			lveModel->bind(commandBuffers[i]);
			lveModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin record command buffer!");

			} // if

		} // for

	} // createCommandBuffers

	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = lveSwapChain.acquireNextImage(&imageIndex); // handles all the synchronization surronding double and triple buffering 

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to aquire the next swapchain image");

		} // if

		result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result != VK_SUCCESS){
			throw std::runtime_error("failed to aquire the submit command buffers");

		} // if 

	} // drawFrame

	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);

	} // ~FirstApp

} // namespace lve