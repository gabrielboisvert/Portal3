#include "../../Source/include/Core/DataStructure/Objects/Empty.h"

Core::Empty::Empty(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform)
	: GameObject(name, cam, model, shader, texture, localTransform)
{
	this->type = "(Empty)";
}

Core::Empty::Empty(Empty& other) : GameObject(other)
{
}

Core::GameObject* Core::Empty::clone()
{
	return new Core::Empty(*this);
}

void Core::Empty::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_EMPTY);
	GameObject::serialize(writer);
	writer.EndObject();
}

const std::string& Core::Empty::getType()
{
	return this->type;
}
