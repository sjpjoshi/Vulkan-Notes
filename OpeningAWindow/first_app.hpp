#pragma once

#include "lve_window.hpp"

namespace lve {
	
	class FirstApp {

	public:
		int static constexpr WIDTH = 800; 
		int static constexpr HEIGHT = 600;
		void run();

	private:
		LveWindow lveWindow{ WIDTH , HEIGHT, "Hello Vulkan!" };

	}; // FirstApp

} // lve