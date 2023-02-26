#pragma once
#include "../../../Core/DataStructure/PhysicsObject.h"

namespace Core
{
	class Platform : public PhysicsObject
	{
	public:
		Platform(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, lm::vec3 pMaxVec, float pDuration = 2, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		Platform(Platform& other);
		void reset();
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		void setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale) override;
		void update(float delta) override;
		GameObject* clone() override;
		virtual void drawTransform(GLFWwindow* window) override;
		const std::string& getType() override;
		void OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;

	private:
		GLFWwindow* window = nullptr;
		lm::vec3 minVec;
		lm::vec3 maxVec;
		float duration;
		float time = 0;
		bool movingForward = true;
	};

}