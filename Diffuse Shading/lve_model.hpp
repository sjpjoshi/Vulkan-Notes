#pragma once

#include "lve_device.hpp"

// libs
#define GLM_FORCE_RADIANS // forces in radians and not degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan uses 0 to 1, openGL uses 1 to 1
#include <glm/glm.hpp>

// stds
#include <vector>
#include <memory>

namespace lve {
	class LveModel {
	
	public:
		
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			// we can either make s2 separate bindings or interleave
			// its simplier to interleave 

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;

			} // operator==

		}; // Vertex

		// this is a temporary builder object storing our vertex and index information until it can be copied over to the model's index and buffer index memory
		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices {};

			void loadModel(const std::string& filepath);

		}; // Data

		LveModel(LveDevice& lveDevice, const LveModel::Builder &builder);
		~LveModel();

		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		static std::unique_ptr<LveModel> createModelFromFile(LveDevice& device, const std::string& filepath);

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