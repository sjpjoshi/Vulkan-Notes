#include "lve_model.hpp"
#include "lve_device.hpp"

//libs
#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader.h>
#include "lve_utils.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace std {
	template<>
	struct hash<lve::LveModel::Vertex> {
		size_t operator() (lve::LveModel::Vertex const& vertex) const {
				size_t seed = 0; // this will store the final hash value
				lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
				return seed;

		} // size_t operator

 	}; //struct hash <lve::LveModel::Vertex>

} // namespace std

namespace lve { 

	LveModel::LveModel(LveDevice& device, const LveModel::Builder &builder) : lveDevice{device} {
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

	std::unique_ptr<LveModel> LveModel::createModelFromFile(LveDevice& device, const std::string& filepath) {
		Builder builder{};
		builder.loadModel(filepath);
		std::cout << "Vertex count: " << builder.vertices.size() << "\n";
		return std::make_unique<LveModel>(device, builder);
		
	} // createModelFromFile

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
		// note: HOST = CPU and DEVICE = GPU
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount; // formula for giving us the total number of bytes 

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory

		); // createBuffer

		void* data;
		vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data); // this creates a region of host memory and maps it to a region of device memory 
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
			vertexBuffer,
			vertexBufferMemory

		); // createBuffer

		// we need to perform a copy operation to move the contents of the staging buffer to the vertex buffer
		lveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);

	} // createVertexBuffers

	void LveModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer)
			return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount; // formula for giving us the total number of bytes 

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		lveDevice.createBuffer(
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingBuffer, 
			stagingBufferMemory
			 
		); // createBuffer

		void* data;
		vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data); // this creates a region of host memory and maps it to a region of device memory 
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

		lveDevice.createBuffer(
			bufferSize, 
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory 

		); // createBuffer

		// we need to perform a copy operation to move the contents of the staging buffer to the vertex buffer
		lveDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);

	} // createIndexBuffers

	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;

	} // getBindingDescriptions

	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) }); 
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });


		return attributeDescriptions;

	} // getAttributeDescriptions

	void LveModel::Builder::loadModel(const std::string& filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) 
			throw std::runtime_error(warn + err);
		
		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]

					}; // vertex.position

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]

					}; // vertex.position

				} // if

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]

					}; // vertex.normal

				} // if (index.normal_index >= 0)

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],

					}; // vertex.normal

				} // if (index.texcoord_index >= 0)

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);

				} // if (uniqueVertices.count(vertex) == 0)

				indices.push_back(uniqueVertices[vertex]);

			} // for (const auto& index : shape.mesh.indices)

		} // for (const auto& shape : shapes)

	} // loadModel

} // lve