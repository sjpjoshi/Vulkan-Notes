#include "lve_model.hpp"

// std
#include <cassert>
#include <cstring>

namespace lve { 

	LveModel::LveModel(LveDevice& device, const std::vector<Vertex>& vertices) : lveDevice{device} {
		createVertexBuffers(vertices);

	} // LveModel

	LveModel::~LveModel() {
		// Vulkan lets us allocate our binding and bufferts separately
		// this is because allocating memory takes time and there is a hard limit with the active allocations and it differs by GPU
		// typically only in the 1000s
		// so if we continue on, as soon as we want some sort of complex model we will run quickly into the max allocation limits
		// the solution is to allocate bigger chunks of memory and parts of them to particular resources
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);

	} // ~LveModel

	void LveModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	} // bind

	void LveModel::draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 1);

	}// draw

	void LveModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount; // formula for giving us the total number of bytes 
		// note: HOST = CPU and DEVICE = GPU
		lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);

		void* data;
		vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data); // this creates a region of host memory and maps it to a region of device memory 
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), vertexBufferMemory);

	} // createVertexBuffers

	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;

	} // getBindingDescriptions

	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions()	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);		
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = 0;
		return attributeDescriptions;

	} // getAttributeDescriptions

} // lve