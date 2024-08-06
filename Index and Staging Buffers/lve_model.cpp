#include "lve_model.hpp"

// std
#include <cassert>
#include <cstring>

namespace lve { 

	LveModel::LveModel(LveDevice& device, const LveModel::Data &builder) : lveDevice{device} {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);

	} // LveModel

	LveModel::~LveModel() {
		// Vulkan lets us allocate our binding and bufferts separately
		// this is because allocating memory takes time and there is a hard limit with the active allocations and it differs by GPU
		// typically only in the 1000s
		// so if we continue on, as soon as we want some sort of complex model we will run quickly into the max allocation limits
		// the solution is to allocate bigger chunks of memory and parts of them to particular resources
		
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);

		if (hasIndexBuffer) {
			vkDestroyBuffer(lveDevice.device(), indexBuffer, nullptr); 
			vkFreeMemory(lveDevice.device(), indexBufferMemory, nullptr);

		} // if

	} // ~LveModel

	void LveModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer)
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	} // bind

	void LveModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer)
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		else
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 1);

	} // draw

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

	void LveModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer)
			return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount; // formula for giving us the total number of bytes 
		// note: HOST = CPU and DEVICE = GPU
		lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexBuffer, indexBufferMemory);

		void* data;
		vkMapMemory(lveDevice.device(), indexBufferMemory, 0, bufferSize, 0, &data); // this creates a region of host memory and maps it to a region of device memory 
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), indexBufferMemory);

	} // createIndexBuffers

	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;

	} // getBindingDescriptions

	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions()	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);		
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position); 

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;

	} // getAttributeDescriptions

} // lve