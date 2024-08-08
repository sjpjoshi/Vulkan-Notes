#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "lve_camera.hpp"
#include "keyboard_movement_controller.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>
#include <chrono>
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
		LveCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		// this model has nothing in it and won't be rendered. It sole purpose is to store the camera's current state
		auto viewerObject = LveGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose()) { // the condition checks if they have noc closed it

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime; // to store next time value

			frameTime = glm::min(frameTime, 10.f);

			cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			glfwPollEvents(); // a window processing events call

			float aspect = lveRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = lveRenderer.beginFrame()) {
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();

			} // if

		} // while

		vkDeviceWaitIdle(lveDevice.device());

	} // run

	void FirstApp::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/Snorlax.obj");

		// we need to make sure our objects are within a Viewing Volume,
		// Viewing Volume: only what is inside the viewing volume is displayed

		auto gameObject = LveGameObject::createGameObject(); 
		gameObject.model = lveModel; 
		gameObject.transform.translation = { .0f, .0f, 4.f }; 
		gameObject.transform.scale = { 3.f, 1.5f, 3.f }; 

		gameObjects.push_back(std::move(gameObject)); 

	} // loadModels

	FirstApp::~FirstApp() {

	} // ~FirstApp

} // namespace lve