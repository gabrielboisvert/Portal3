#include "../../../Source/include/Core/DataStructure/InteractibleObject.h"
#include "../../../include/Application.h"

Core::InteractibleObject::InteractibleObject(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize)
	: PhysicsObject(name, physic, cam, model, shader, texture, localTransform, rigidBodySize), window(&window)
{
}

Core::InteractibleObject::InteractibleObject(InteractibleObject& other) : PhysicsObject(other)
{
	this->window = other.window;
}

void Core::InteractibleObject::updateGlobal()
{
	PhysicsObject::updateGlobal();
	this->rigidBody->getRigidBody()->setRestitution(0.8f);
}

void Core::InteractibleObject::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	PhysicsObject::serialize(writer);
}

void Core::InteractibleObject::teleport(Portal& portal)
{
	lm::vec3 portalFoward = portal.getForward();
	lm::vec3 newPos = portal.getPosition();

	this->position = newPos + (portalFoward * (2 * this->scale.X() ) );

	btVector3 velo = this->getRigidBody()->getRigidBody()->getLinearVelocity();
	lm::vec3 d(velo.x(), velo.y(), velo.z());
	lm::vec3 r = d.length() * portalFoward;
	float x = r.X();
	float y = r.Y();
	float z = r.Z();
	this->getRigidBody()->getRigidBody()->setLinearVelocity(btVector3(x, y, z));

	this->updateLocal();
}

void Core::InteractibleObject::OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionEnter(me, other, otherphysicsobj, manifold);

	Application* app = (Application*)glfwGetWindowUserPointer(window);

	Player* player = app->getPlayer();
	Portal* portal = dynamic_cast<Portal*>(otherphysicsobj);
	if (portal != nullptr && portal->getIsLinked())
		this->teleport((otherphysicsobj == player->getPortal(0) ? *player->getPortal(1) : *player->getPortal(0)));
}

void Core::InteractibleObject::OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionStay(me, other, otherphysicsobj, manifold);

	Application* app = (Application*)glfwGetWindowUserPointer(window);

	Player* player = app->getPlayer();
	Portal* portal = dynamic_cast<Portal*>(otherphysicsobj);
	if (portal != nullptr && portal->getIsLinked())
		this->teleport((otherphysicsobj == player->getPortal(0) ? *player->getPortal(1) : *player->getPortal(0)));
}
