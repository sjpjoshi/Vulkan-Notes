#pragma once

#include "lve_model.hpp";
#include <memory>

namespace lve {

	struct Transform2dComponent {
		glm::vec2 translation;
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;

		glm::mat2 mat2() { 

			// s: sin
			// c: cos
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{

				{c, s},
				{-s, c}

			}; // rotMatrix

			glm::mat2 scaleMat{

				{scale.x, .0f},
				{.0f, scale.y}

			}; // scaleMat

			return scaleMat * rotMatrix;

		} // mat2()

	}; // Transform2dComponent

	class LveGameObject {
	public:
		using id_t = unsigned int;
		static LveGameObject createGameObject() {
			static id_t currentId = 0;
			return LveGameObject(currentId++);

		} // createGameObject

		// delete the copy constructors
		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		id_t getId() { return id; } // getId
		
		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};

		Transform2dComponent transform2d{};

	private:
		LveGameObject(id_t objId) : id{ objId } {}

		// we want each game object to be constructed with its own unique ID
		id_t id;

	}; // LveGameObject

} // lve