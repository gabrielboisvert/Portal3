#pragma once
#include "../../../Core/DataStructure/InteractibleObject.h"
#include <LibMaths/Vec3/Vec3.h>
#include "../../../../include/Physics/PhysicsEngine.h"
#include "../../../../include/Application.h"

namespace Core
{
	class Turret : public InteractibleObject
	{
	public:
		Turret(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const float& radius, const float& viewAngle, const lm::mat4& localTransform, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		Turret(Turret& other);
		void shoot(float deltaTime);
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		GameObject* clone() override;
		const std::string& getType() override;
		void update(float deltaTime) override;
		void raycasting();
		void setRadius(const float& radius);
		virtual void drawTransform(GLFWwindow* window) override;
		void kill();

	private:
		bool isAlive = true;
		bool hasViewPlayer = false;
		float radius;
		float viewAngle;
		float fireRate = 1;
		float time = 0;
		float deathTime = 0;
		lm::vec3 deathDir;
		irrklang::ISound* soundEffect = nullptr;
		
		void OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
	};
}