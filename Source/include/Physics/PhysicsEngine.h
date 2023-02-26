#pragma once
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/BroadphaseCollision/btBroadphaseInterface.h"
#include "BulletDynamics/ConstraintSolver/btBatchedConstraints.h"
#include <vector>
#include "../Core/Debug/PhysicDebugDrawer.h"
#include <set>
#include <functional>

namespace Physics
{
	using CollisionEnterCallback = std::function<void(btRigidBody*, btRigidBody*, void*, btPersistentManifold*)>;
	using CollisionExitCallback = std::function<void(btRigidBody*, btRigidBody*, void*, btPersistentManifold*)>;
	
	struct CollisionCallbacks
	{
		CollisionEnterCallback enter;
		CollisionEnterCallback stay;
		CollisionExitCallback exit;
		void* physicsObject;
	};

	enum CollisionGroups
	{
		Default = 1,
		Player = 2,
		Portal = 3
	};

	class PhysicsEngine
	{
	private:
		btDynamicsWorld* world = nullptr;
		btDispatcher* dispatcher = nullptr;
		btCollisionConfiguration* collConfig = nullptr;
		btBroadphaseInterface* broadPhase = nullptr;
		btConstraintSolver* solver = nullptr;
		std::vector<btRigidBody*> bodies;
		Core::PhysicDebugDrawer* debugDrawer = nullptr;
		using CollisionSet = std::set<std::tuple<const btCollisionObject*, const btCollisionObject*, const btPersistentManifold*>>;
		CollisionSet CollisionPairs;

	public:
		PhysicsEngine(GLFWwindow* window);
		~PhysicsEngine();
		
		btDynamicsWorld* getWorld();
		btDispatcher* getDispatcher();
		btCollisionConfiguration* getCollConfig();
		btBroadphaseInterface* getBroadPhase();
		btConstraintSolver* getSolver();
		std::vector<btRigidBody*>& getRigidBodies();
		void update(float deltaTime);
		void reset();
		btRigidBody* PhysicsEngine::addBox(const lm::vec3& position, const lm::vec3& size, float mass, CollisionCallbacks* callbacks, int tag = 0);
		void CheckForCollisionEvents();
		CollisionSet& getCollisionSet();
	};

}
