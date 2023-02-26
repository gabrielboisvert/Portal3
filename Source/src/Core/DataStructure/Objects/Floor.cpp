#include "../../Source/include/Core/DataStructure/Objects/Floor.h"

Core::Floor::Floor(const std::string& name, Physics::PhysicsEngine& physic, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize)
	: PhysicsObject(name, physic, cam, model, shader, texture, localTransform)
{
	this->type = "(Floor)";
	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_STATIC,
											lm::vec3(localTransform[3][0], localTransform[3][1], localTransform[3][2]),
											rigidBodySize,
											0, physic, &Callbacks);
}

Core::Floor::Floor(Core::Floor& other) : Core::PhysicsObject(other)
{
}

void Core::Floor::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_FLOOR);
	PhysicsObject::serialize(writer);
	writer.EndObject();
}

Core::GameObject* Core::Floor::clone()
{
	return new Core::Floor(*this);
}

const std::string& Core::Floor::getType()
{
	return this->type;
}