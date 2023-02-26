#include "../../Source/include/Core/DataStructure/Objects/CompanionCube.h"
#include "../../../../include/Application.h"
#include <cmath>
Core::CompanionCube::CompanionCube(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize)
   : InteractibleObject(name, physic, window, cam, model, shader, texture, localTransform)
{
	this->type = "(CompanionCube)";
	

	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_DYNAMIC, 
												lm::vec3(localTransform[3][0], localTransform[3][1], localTransform[3][2]),
												rigidBodySize,
												100.f, physic, &Callbacks);
	this->rigidBody->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->rigidBody->getRigidBody()->applyGravity();
}

Core::CompanionCube::CompanionCube(CompanionCube& other) : InteractibleObject(other)
{
	this->window = other.window;
	this->rigidBody->setType(Physics::RigidBody::TypeRigidBody::E_DYNAMIC);
	this->rigidBody->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->rigidBody->getRigidBody()->applyGravity();
}

void Core::CompanionCube::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.Key("ClassType");
	writer.Int(E_COMPANION_CUBE);
	InteractibleObject::serialize(writer);
	writer.EndObject();
}

Core::GameObject* Core::CompanionCube::clone()
{
	return new Core::CompanionCube(*this);
}

const std::string& Core::CompanionCube::getType()
{
	return this->type;
}