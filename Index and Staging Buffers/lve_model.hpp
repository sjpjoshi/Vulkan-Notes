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
			glm::vec3 position;
			glm::vec3 color;

			// we can either make 2 separate bindings or interleave
			// its simplier to interleave 

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		}; // Vertex

		// while we could call this Builder, it would sound too much like the Builder Design Pattern and this is not the same thing
		// this is a temporary builder object storing our vertex and index information until it can be copied over to the model's index and buffer index memory
		struct Data {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices {};

		}; // Data

		LveModel(LveDevice& lveDevice, const LveModel::Data &builder);
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);


	private:
		LveDevice& lveDevice;

		// two separate objects, we are in charge of memory management here
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer; 
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;


	}; // LveModel

} // lve