#include "first_app.hpp"

namespace lve {

	// this is a check to see if the user has closed the window
	void FirstApp::run() {
		while (!lveWindow.shouldClose()) { // the condition checks if they have noc closed it
			glfwPollEvents(); // a window processing events call

		} // while

	} // run

} // lve