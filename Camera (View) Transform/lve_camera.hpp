#pragma once

// libs
#define GLM_FORCE_RADIANS // forces in radians and not degrees
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan uses 0 to 1, openGL uses 1 to 1
#include <glm/glm.hpp>

namespace lve {
	class LveCamera {
	private: 
		glm::mat4 projectionMatrix{ 1.f };


	public:
		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void setPerspectiveProjection(float fov_Y, float aspect, float near, float far);

		const glm::mat4& getProjection() const { return projectionMatrix; } // getProjection

	}; // lveCamera

} // lve