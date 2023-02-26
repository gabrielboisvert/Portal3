#include "../../include/Physics/PhysicsEngine.h"

Physics::PhysicsEngine::PhysicsEngine(GLFWwindow* window)
{
	this->debugDrawer = new Core::PhysicDebugDrawer(window);

	collConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collConfig);
	broadPhase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collConfig);
	world->setGravity(btVector3(0, -9.8, 0));
	world->setDebugDrawer(this->debugDrawer);
}

void Physics::PhysicsEngine::update(float deltaTime)
{
	if (deltaTime == 0)
		return;

	this->world->stepSimulation(deltaTime);
	this->CheckForCollisionEvents();
}

void Physics::PhysicsEngine::reset()
{
	for (unsigned int i = 0; i < this->bodies.size(); i++)
		this->world->removeRigidBody(this->bodies[i]);
	this->bodies.clear();
	this->CollisionPairs.clear();
}

btRigidBody* Physics::PhysicsEngine::addBox(const lm::vec3& position, const lm::vec3& size, float mass, Physics::CollisionCallbacks* callbacks, int tag)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.X(), position.Y(), position.Z()));
	
	btBoxShape* box = new btBoxShape(btVector3(size.X(), size.Y(), size.Z()));

	btVector3 inertia(0, 0, 0);
	if (mass != 0 && box != nullptr)
		box->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);
	btRigidBody* rb = new btRigidBody(info);
	
	CollisionCallbacks* cb = new CollisionCallbacks();
	cb->enter = callbacks->enter;
	cb->stay = callbacks->stay;
	cb->exit = callbacks->exit;
	cb->physicsObject = (void*)callbacks->physicsObject;
	rb->setUserPointer(cb);
	if(tag != 0)
		this->world->addRigidBody(rb, CollisionGroups::Player, btBroadphaseProxy::AllFilter);
	else
		this->world->addRigidBody(rb, CollisionGroups::Default, btBroadphaseProxy::AllFilter);

	this->bodies.push_back(rb);
	
	return rb;
}

void Physics::PhysicsEngine::CheckForCollisionEvents()
{
	btDispatcher* dispatcher = world->getDispatcher();
	if (!dispatcher)
		return;

	CollisionSet NewCollisions;
	int numManifolds = dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);

		if (!contactManifold || contactManifold->getNumContacts() <= 0)
			continue;

		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		bool const Swapped = obA > obB;
		const btCollisionObject* RigidA = Swapped ? obA : obB;
		const btCollisionObject* RigidB = Swapped ? obB : obA;

		NewCollisions.emplace(RigidA, RigidB, contactManifold);
	}

	CollisionSet differenceSet;
	std::set_symmetric_difference(CollisionPairs.begin(), CollisionPairs.end(), NewCollisions.begin(), NewCollisions.end(), std::inserter(differenceSet, differenceSet.begin()));
	
	CollisionSet stayColision;
	std::set_union(CollisionPairs.begin(), CollisionPairs.end(), NewCollisions.begin(), NewCollisions.end(), std::inserter(stayColision, stayColision.begin()));
	for (std::tuple<const btCollisionObject*, const btCollisionObject*, const btPersistentManifold*> const& pair : stayColision)
	{
		CollisionCallbacks* Callbacks = (CollisionCallbacks*)std::get<0>(pair)->getUserPointer();
		CollisionCallbacks* Callbacks2 = (CollisionCallbacks*)std::get<1>(pair)->getUserPointer();
		btRigidBody* body0 = (btRigidBody*)std::get<0>(pair);
		btRigidBody* body1 = (btRigidBody*)std::get<1>(pair);
		btPersistentManifold* manifold = (btPersistentManifold*)std::get<2>(pair);
		if (Callbacks->stay)
			Callbacks->stay(body0, body1, Callbacks2->physicsObject, manifold);

		if (Callbacks2->stay)
			Callbacks2->stay(body1, body0, Callbacks->physicsObject, manifold);
	}

	for (std::tuple<const btCollisionObject *, const btCollisionObject *, const btPersistentManifold*> const& pair : differenceSet)
	{
		if (CollisionPairs.find(pair) == CollisionPairs.end())
		{
			CollisionCallbacks* Callbacks = (CollisionCallbacks*)std::get<0>(pair)->getUserPointer();
			CollisionCallbacks* Callbacks2 = (CollisionCallbacks*)std::get<1>(pair)->getUserPointer();
			btRigidBody* body0 = (btRigidBody*)std::get<0>(pair);
			btRigidBody* body1 = (btRigidBody*)std::get<1>(pair);
			btPersistentManifold* manifold = (btPersistentManifold*)std::get<2>(pair);
			if (Callbacks->enter)
				Callbacks->enter(body0, body1, Callbacks2->physicsObject, manifold);
			
			if (Callbacks2->enter)
				Callbacks2->enter(body1, body0, Callbacks->physicsObject, manifold);

			CollisionPairs.emplace(pair);
		}
		else
		{
			CollisionCallbacks* Callbacks = (CollisionCallbacks*)std::get<0>(pair)->getUserPointer();
			CollisionCallbacks* Callbacks2 = (CollisionCallbacks*)std::get<1>(pair)->getUserPointer();
			btRigidBody* body0 = (btRigidBody*)std::get<0>(pair);
			btRigidBody* body1 = (btRigidBody*)std::get<1>(pair);
			btPersistentManifold* manifold = (btPersistentManifold*)std::get<2>(pair);
			if (Callbacks && Callbacks2)
			{

				if (Callbacks->exit)
					Callbacks->exit(body0, body1, Callbacks2->physicsObject, manifold);


				if (Callbacks2->exit)
					Callbacks2->exit(body1, body0, Callbacks->physicsObject, manifold);
			}
		}
	}
	CollisionPairs = NewCollisions;
}

Physics::PhysicsEngine::CollisionSet& Physics::PhysicsEngine::getCollisionSet()
{
	return this->CollisionPairs;
}

Physics::PhysicsEngine::~PhysicsEngine()
{
	delete this->dispatcher;
	delete this->collConfig;
	delete this->solver;
	delete this->world;
	delete this->broadPhase;
	for (unsigned int i = 0; i < this->bodies.size(); i++)
		delete bodies[i];
	delete this->debugDrawer;
}

btDynamicsWorld* Physics::PhysicsEngine::getWorld()
{
	return this->world;
}

btDispatcher* Physics::PhysicsEngine::getDispatcher()
{
	return this->dispatcher;
}

btCollisionConfiguration* Physics::PhysicsEngine::getCollConfig()
{
	return this->collConfig;
}

btBroadphaseInterface* Physics::PhysicsEngine::getBroadPhase()
{
	return this->broadPhase;
}

btConstraintSolver* Physics::PhysicsEngine::getSolver()
{
	return this->solver;
}

std::vector<btRigidBody*>& Physics::PhysicsEngine::getRigidBodies()
{
	return this->bodies;
}
