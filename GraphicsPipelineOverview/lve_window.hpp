#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve {
	
	class LveWindow {
	
	public:
		LveWindow(int w, int h, std::string name);
		~LveWindow();

		// we want to destroy the copy constructors
		// if we do not destroy the copy constructors we will have a dangling pointer
		// we dont want to copy our lve window, if it is the shared glfw window will be terminated
		LveWindow(const LveWindow &) = delete;
		LveWindow &operator=(const LveWindow &) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

	private:
		GLFWwindow *window;
		void  initWindow();
		const int width, height;
		std::string windowName;

	}; // LveWindow

} // lve