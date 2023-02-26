#pragma once
#include "../../../Core/DataStructure/InteractibleObject.h"
#include "../../../../include/Physics/PhysicsEngine.h"
#include "Turret.h"
#include "CompanionCube.h"


namespace Core
{
	class Button : public PhysicsObject
	{
	public:
		Button(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		Button(Core::Button& other);
		~Button();
		bool isPressed();
		const std::string& getType() override;
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		GameObject* clone() override;
		void addDoor(Core::GameObject& door);
		void removeDoor(Core::GameObject& door);
		void OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold = nullptr) override;
		void OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
	
	private:
		GLFWwindow* window = nullptr;
		bool pressed = false;
		std::list<Core::GameObject*> linkedDoor;
		int colisionCount = 0;
	};
}
