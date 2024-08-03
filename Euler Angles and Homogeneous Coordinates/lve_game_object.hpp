#pragma once

#include "lve_model.hpp";
#include <memory>

// libs
#include <glm/gtc/matrix_transform.hpp>



namespace lve {

	struct TransformComponent {
		glm::vec3 translation;
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation;


		glm::mat4 mat4() {
			auto transform = glm::translate(glm::mat4{ 1.f }, translation); // this creates our 4x4 translation matrix
			transform = glm::scale(transform, scale);
			return transform;

		} // mat4

	}; // TransformComponent

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