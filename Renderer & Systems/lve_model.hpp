#pragma once

#include "lve_device.hpp"

// libs
#define GLM_FORCE_RADIANS // forces in radians and not degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan uses 0 to 1, openGL uses 1 to 1
#include <glm/glm.hpp>

// stds
#include <vector>


namespace lve {
	class LveModel {
	
	public:
		
		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;

			// we can either make 2 separate bindings or interleave
			// its simplier to interleave 

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		}; // Vertex

		LveModel(LveDevice& lveDevice, const std::vector<Vertex>& vertices);
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);


	private:
		// Variables
		LveDevice& lveDevice;

		// two separate objects, we are in charge of memory management here
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		uint32_t vertexCount;

		void createVertexBuffers(const std::vector<Vertex>& vertices);

	}; // LveModel

} // lve