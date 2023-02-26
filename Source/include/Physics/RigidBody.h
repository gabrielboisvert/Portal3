#pragma once
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/BroadphaseCollision/btBroadphaseInterface.h"
#include "BulletDynamics/ConstraintSolver/btBatchedConstraints.h"
#include "../../include/Physics/PhysicsEngine.h"
#include "LibMaths/Mat4/Mat4.h"


namespace Physics
{
	class RigidBody
	{
	public:
		enum class TypeRigidBody
		{
			E_DYNAMIC,
			E_KINEMATIC,
			E_STATIC
		};

		RigidBody(TypeRigidBody type, const lm::vec3& position, const lm::vec3& size, float mass, PhysicsEngine& physicEngine, CollisionCallbacks* callbacks = nullptr, bool isPlayer = false);
		RigidBody(RigidBody& other, CollisionCallbacks* callbacks = nullptr);
		~RigidBody();

		lm::vec3 getPosition();
		void Physics::RigidBody::setTransform(const lm::vec3& position, const lm::mat4& rotation, const lm::vec3& scale);
		lm::mat4 Physics::RigidBody::btScalar2mat4(btScalar* matrix);
		void setType(TypeRigidBody type);
		btRigidBody* getRigidBody();
		lm::vec3 getAngles(lm::mat4 source);
		lm::mat4 getmatrix();
		PhysicsEngine* getEngine();
		lm::vec3& getSize();

	private:
		PhysicsEngine* physicEngine;
		btRigidBody* rigidbody = nullptr;
		TypeRigidBody type = TypeRigidBody::E_DYNAMIC;
		CollisionCallbacks* callback;
		lm::vec3 position;
		lm::vec3 size;
		float mass;
		int playertag = 2;
	};
}