#pragma once
#include "../../Core/DataStructure/GameObject.h"
#include "../../Physics/RigidBody.h"
#include "../../Physics/PhysicsEngine.h"
#include <functional>

namespace Core
{
	class PhysicsObject : public GameObject
	{
	public:
		PhysicsObject(const std::string& name, Physics::PhysicsEngine& physic, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		PhysicsObject(PhysicsObject& other);
		virtual ~PhysicsObject();
		void updateGlobal() override;
		void setLocal(const lm::mat4& local) override;
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		void virtual update(float deltaTime) override;
		void setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale) override;
		Physics::RigidBody* getRigidBody();
		virtual void OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* persistent = nullptr);
		virtual void OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* persistent = nullptr);
		virtual void OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* Physicsobj, btPersistentManifold* persistent = nullptr);
		virtual void drawRessource(GLFWwindow* window) override;

	protected:
		Physics::RigidBody* rigidBody = nullptr;
		Physics::PhysicsEngine* physic = nullptr;
		Physics::CollisionCallbacks Callbacks;

	};
}
