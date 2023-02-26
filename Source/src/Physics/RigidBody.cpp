#include "../../include/Physics/RigidBody.h"
#include "../../include/Application.h"
#include <cmath>

Physics::RigidBody::RigidBody(Physics::RigidBody::TypeRigidBody type, const lm::vec3& position, const lm::vec3& size, float mass, PhysicsEngine& physicEngine, 
	CollisionCallbacks* callbacks, bool isPlayer) : physicEngine(&physicEngine)
{
	this->type = type;
	this->position = position;
	this->size = size;
	this->mass = mass;
	if (!isPlayer)
		rigidbody = physicEngine.addBox(this->position, this->size, mass, callbacks);
	else
		rigidbody = physicEngine.addBox(this->position, this->size, mass, callbacks, playertag);

	callbacks = callbacks;

	switch (this->type)
	{
	case TypeRigidBody::E_DYNAMIC: rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
		break;

	case TypeRigidBody::E_KINEMATIC: rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		break;

	case TypeRigidBody::E_STATIC: rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		break;

	default:
		break;
	}
}

Physics::RigidBody::RigidBody(RigidBody& other, CollisionCallbacks* callbacks)
{
	this->physicEngine = other.physicEngine;
	this->type = other.type;
	this->position = other.position;
	this->size = other.size;
	this->mass = other.mass;

	this->rigidbody = this->physicEngine->addBox(this->position, this->size, mass, callbacks);
}

Physics::RigidBody::~RigidBody()
{
	std::vector<btRigidBody*>::iterator it = std::find(this->physicEngine->getRigidBodies().begin(), this->physicEngine->getRigidBodies().end(), this->rigidbody);
	this->physicEngine->getRigidBodies().erase(it);
	this->physicEngine->getWorld()->removeRigidBody(this->rigidbody);

	for (std::set<std::tuple<const btCollisionObject*, const btCollisionObject*, const btPersistentManifold*>>::iterator it = this->physicEngine->getCollisionSet().begin(); it != this->physicEngine->getCollisionSet().end();)
		if (std::get<0>(*it) == (btCollisionObject*)this->getRigidBody() || std::get<1>(*it) == (btCollisionObject*)this->getRigidBody())
			it = this->physicEngine->getCollisionSet().erase(it);
		else
			++it;

	delete this->rigidbody;
}

lm::vec3 Physics::RigidBody::getPosition()
{
	btTransform transform;
	btScalar worldmatrix[16];
	transform = rigidbody->getWorldTransform();
	transform.getOpenGLMatrix(worldmatrix);
	lm::mat4 b = this->btScalar2mat4(worldmatrix);
	this->position = lm::vec3(b[3][0], b[3][1], b[3][2]);
	
	return position;
}

lm::mat4 Physics::RigidBody::getmatrix()
{
	btTransform transform;
	btScalar worldmatrix[16];
	transform = rigidbody->getWorldTransform();
	transform.getOpenGLMatrix(worldmatrix);
	lm::mat4 b = this->btScalar2mat4(worldmatrix);
	return b;
}

Physics::PhysicsEngine* Physics::RigidBody::getEngine()
{
	return physicEngine;
}

lm::vec3& Physics::RigidBody::getSize()
{
	return this->size;
}

void Physics::RigidBody::setTransform(const lm::vec3& position, const lm::mat4& rotation, const lm::vec3& scale)
{
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(position.X(), position.Y(), position.Z()));
	tr.setBasis(btMatrix3x3(rotation[0][0], rotation[1][0], rotation[2][0], rotation[0][1], rotation[1][1], rotation[2][1], rotation[0][2], rotation[1][2], rotation[2][2]));


	if (this->type == TypeRigidBody::E_KINEMATIC)
		this->rigidbody->getMotionState()->setWorldTransform(tr);
	else
		this->rigidbody->setWorldTransform(tr);

	btVector3 v = this->rigidbody->getLinearVelocity();

	const lm::vec3 finalScale = scale.scaled(this->size);

	btBoxShape* box = new btBoxShape(btVector3(finalScale.X(), finalScale.Y(), finalScale.Z()));
	delete this->rigidbody->getCollisionShape();
	this->rigidbody->setCollisionShape(box);
	
	this->rigidbody->setLinearVelocity(v);
}

lm::mat4 Physics::RigidBody::btScalar2mat4(btScalar* matrix)
{
	lm::vec4 a(matrix[0], matrix[1], matrix[2], matrix[3]);
	lm::vec4 b(matrix[4], matrix[5], matrix[6], matrix[7]);
	lm::vec4 c(matrix[8], matrix[9], matrix[10], matrix[11]);
	lm::vec4 d(matrix[12], matrix[13], matrix[14], matrix[15]);

	return lm::mat4(a, b, c, d);
}

void Physics::RigidBody::setType(TypeRigidBody type)
{
	this->type = type;

	switch (this->type)
	{
	case TypeRigidBody::E_DYNAMIC: rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
		break;

	case TypeRigidBody::E_KINEMATIC: rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		break;

	case TypeRigidBody::E_STATIC: rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		break;

	default:
		break;
	}
}

btRigidBody* Physics::RigidBody::getRigidBody()
{
	return this->rigidbody;
}

lm::vec3 Physics::RigidBody::getAngles(lm::mat4 source)
{
	double thetaX, thetaY, thetaZ = 0.0;
	thetaX = asin(source[1][2]);

	if (thetaX < (3.1415f / 2))
	{
		if (thetaX > (-3.1415f / 2))
		{
			thetaZ = atan2(-source[1][0], source[1][1]);
			thetaY = atan2(-source[0][2], source[2][2]);
		}
		else
		{
			thetaY = -atan2(-source[2][0], source[0][0]);
			thetaZ = 0;
		}
	}
	else
	{
		thetaY = atan2(source[2][0], source[0][0]);
		thetaZ = 0;
	}

	lm::vec3 angles(thetaX * 180 / 3.1415f, thetaY * 180 / 3.1415f, thetaZ * 180 / 3.1415f);

	return angles;
}
