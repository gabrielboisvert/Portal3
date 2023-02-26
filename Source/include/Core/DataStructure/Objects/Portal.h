#pragma once
#include "../../../Core/DataStructure/PhysicsObject.h"
#include "../../../LowRenderer/Camera.h"

namespace Core
{
	class Portal : public PhysicsObject
	{
	public:
		Portal(const std::string& name, Physics::PhysicsEngine& physic, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* textureConnected, Resources::Texture* textureNotConnected, const lm::mat4& localTransform);
		~Portal();
		GameObject* clone() override { return nullptr; };
		const std::string& getType() override { return this->type; };
		void draw() override;
		void drawFrame();
		bool& getIsActif();
		bool& getIsLinked();
		void setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale) override;
		void updateGlobal() override;
		void rotate(const lm::vec3& normal, float& yaw);
		lm::mat4& getPortalView();
		void OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		lm::vec3 getNormal();

	private:
		lm::mat4 frame;

		lm::mat4 portalView;
		bool isActif = false;
		bool isLinked = false;

		Resources::Texture* textureConnected = nullptr;
		Resources::Texture* textureNotConnected = nullptr;
	};

}