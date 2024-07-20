#include "lve_pipline.hpp"

// std
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace lve {
	LvePipeline::LvePipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
		createGraphicsPipeline(vertFilePath, fragFilePath);


	} // LvePipeline

	std::vector<char> LvePipeline::readFile(const std::string& filePath) {
		std::ifstream file{ filePath, std::ios::ate | std::ios::binary };
		// std::ios::ate -> we seek the files immediately 
		// std::ios::binary -> reads file in as a binary

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file: " + filePath);

		} // if

		size_t fileSize = static_cast<size_t>(file.tellg()); // already at the end of the file, we only want the last pos, which is the file size
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;

	} // readFile

	void LvePipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
		// wont use til later
		auto vertCode = readFile(vertFilePath);
		auto fragCode = readFile(fragFilePath);
		std::cout << "Vertex Shader Code Size: " << vertCode.size() << "\n";
		std::cout << "Fragment Shader Code Size: " << fragCode.size() << "\n";

	} // createGraphicsPipeline

} // lve