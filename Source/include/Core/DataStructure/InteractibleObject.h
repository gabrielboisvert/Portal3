#pragma once
#include "../../Core/DataStructure/PhysicsObject.h"
#include "Objects/Portal.h"

namespace Core
{
	class InteractibleObject : public PhysicsObject
	{
	public:
		InteractibleObject(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		InteractibleObject(InteractibleObject& other);
		void updateGlobal() override;
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		void virtual teleport(Portal& other);
		void virtual OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void virtual OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;

	protected:
		GLFWwindow* window = nullptr;
	};
}