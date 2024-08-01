#include "first_app.hpp"
#include "simple_render_system.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>

// libs
#define GLM_FORCE_RADIANS // forces in radians and not degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan uses 0 to 1, openGL uses 1 to 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {
	FirstApp::FirstApp() {
	    loadGameObjects();
		
	} // FirstApp

	// this is a check to see if the user has closed the window
	void FirstApp::run() {

		SimpleRenderSystem simpleRenderSystem(lveDevice, lveRenderer.getSwapChainRenderPass());
		while (!lveWindow.shouldClose()) { // the condition checks if they have noc closed it
			glfwPollEvents(); // a window processing events call
			if (auto commandBuffer = lveRenderer.beginFrame()) {
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();

			} // if

		} // while

		vkDeviceWaitIdle(lveDevice.device());

	} // run

	void FirstApp::loadGameObjects() {
		
		std::vector<LveModel::Vertex> vertices{
			{ {0.0f, -0.5f }, {1.0f, 0.0f, 0.0f } }, // red
			{{ 0.5f, 0.5f  }, {0.0f, 1.0f, 0.0f } }, // blue
			{{ -0.5f, 0.5f }, {0.0f, 0.0f, 1.0f } }, // green

		}; // vertices

		auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
		auto triangle = LveGameObject::createGameObject();
		triangle.model = lveModel;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.f, .5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();
		gameObjects.push_back(std::move(triangle));

	} // loadModels

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

	FirstApp::~FirstApp() {

	} // ~FirstApp

} // namespace lve