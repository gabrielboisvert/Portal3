
#include "../../../../include/Core/DataStructure/Objects/Light.h"
#include "../../../../include/Application.h"

Core::DirectionalLight::DirectionalLight(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, LowRenderer::DirectionalLight& light) : Core::GameObject(name, cam, model, shader, texture, localTransform), light(&light)
{
	this->type = "(Directional Light)";
	shader->addDirLights(&light);
}

Core::DirectionalLight::DirectionalLight(DirectionalLight& other) : GameObject(other)
{
}

Core::DirectionalLight::~DirectionalLight()
{
	this->shader->removeDirLights(this->light);
}

void Core::DirectionalLight::update(float delta)
{
}

Core::GameObject* Core::DirectionalLight::clone()
{
	return new DirectionalLight(*this);
}

std::string& Core::DirectionalLight::getType()
{
	return this->type;
}

void Core::DirectionalLight::drawTransform(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.25));

	ImGui::Begin("Transform", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	ImGui::DragFloat3("Direction", &this->light->getDirection()[0], 0.1f, -1, 1, "%.3f");
	ImGui::DragFloat4("Diffuse color", &this->light->getDiffuse()[0], 0.1f, 0, 1, "%.3f");
	ImGui::DragFloat4("Ambient color", &this->light->getAmbient()[0], 0.1f, 0, 1, "%.3f");
	ImGui::DragFloat4("Specular color", &this->light->getSpecular()[0], 0.1f, 0, 1, "%.3f");

	ImGui::End();

	this->position = this->light->getDirection();
}

void Core::DirectionalLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_DIRECTIONAL_LIGHT);
	GameObject::serialize(writer);

	writer.Key("Diffuse");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getDiffuse().X());
	writer.Double(this->light->getDiffuse().Y());
	writer.Double(this->light->getDiffuse().Z());
	writer.Double(this->light->getDiffuse().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Ambient");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getAmbient().X());
	writer.Double(this->light->getAmbient().Y());
	writer.Double(this->light->getAmbient().Z());
	writer.Double(this->light->getAmbient().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Specular");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getSpecular().X());
	writer.Double(this->light->getSpecular().Y());
	writer.Double(this->light->getSpecular().Z());
	writer.Double(this->light->getSpecular().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.EndObject();
}

void Core::DirectionalLight::setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale)
{
	this->position = this->light->getDirection() = position;
}



Core::PointLight::PointLight(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, LowRenderer::PointLight& light) : Core::GameObject(name, cam, model, shader, texture, localTransform), light(&light)
{
	this->type = "(Point Light)";
	shader->addPointLights(&light);
}

Core::PointLight::PointLight(PointLight& other) : GameObject(other)
{
}

Core::PointLight::~PointLight()
{
	this->shader->removePointLights(this->light);
}

void Core::PointLight::update(float delta)
{
}

void Core::PointLight::drawTransform(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.25));

	ImGui::Begin("Transform", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	ImGui::DragFloat3("Position", &this->light->getPosition()[0], 0.2f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat4("Diffuse color", &this->light->getDiffuse()[0], 0.1f, 0, 1, "%.3f");
	ImGui::DragFloat4("Ambient color", &this->light->getAmbient()[0], 0.1f, 0, 1, "%.3f");
	ImGui::DragFloat4("Specular color", &this->light->getSpecular()[0], 0.1f, 0, 1, "%.3f");

	ImGui::DragFloat("Constant", &this->light->getConstant(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat("Linear", &this->light->getLinear(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat("Quadratic", &this->light->getQuadratic(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");

	ImGui::End();

	this->position = this->light->getPosition();
	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
}

Core::GameObject* Core::PointLight::clone()
{
	return new PointLight(*this);
}

std::string& Core::PointLight::getType()
{
	return this->type;
}

void Core::PointLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_POINT_LIGHT);
	GameObject::serialize(writer);

	writer.Key("Diffuse");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getDiffuse().X());
	writer.Double(this->light->getDiffuse().Y());
	writer.Double(this->light->getDiffuse().Z());
	writer.Double(this->light->getDiffuse().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Ambient");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getAmbient().X());
	writer.Double(this->light->getAmbient().Y());
	writer.Double(this->light->getAmbient().Z());
	writer.Double(this->light->getAmbient().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Specular");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getSpecular().X());
	writer.Double(this->light->getSpecular().Y());
	writer.Double(this->light->getSpecular().Z());
	writer.Double(this->light->getSpecular().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Constant");
	writer.Double(this->light->getConstant());

	writer.Key("Linear");
	writer.Double(this->light->getLinear());

	writer.Key("Quadratic");
	writer.Double(this->light->getQuadratic());

	writer.EndObject();
}

void Core::PointLight::setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale)
{
	this->position = this->light->getPosition() = position;
}



Core::SpotLight::SpotLight(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, LowRenderer::SpotLight& light) : Core::GameObject(name, cam, model, shader, texture, localTransform), light(&light)
{
	this->type = "(Spot Light)";
	shader->addSpotLights(&light);
}

Core::SpotLight::SpotLight(SpotLight& other) : GameObject(other)
{
}

Core::SpotLight::~SpotLight()
{
	this->shader->removeSpotLights(this->light);
}

void Core::SpotLight::update(float delta)
{
}

void Core::SpotLight::drawTransform(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.25));

	ImGui::Begin("Transform", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	ImGui::DragFloat3("Position", &this->light->getPosition()[0], 0.2f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat3("Direction", &this->light->getDirection()[0], 0.1f, -1, 1, "%.3f");

	ImGui::DragFloat4("Diffuse color", &this->light->getDiffuse()[0], 0.1f, 0, 1, "%.3f");
	ImGui::DragFloat4("Ambient color", &this->light->getAmbient()[0], 0.1f, 0, 1, "%.3f");
	ImGui::DragFloat4("Specular color", &this->light->getSpecular()[0], 0.1f, 0, 1, "%.3f");

	ImGui::DragFloat("Constant", &this->light->getConstant(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat("Linear", &this->light->getLinear(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat("Quadratic", &this->light->getQuadratic(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");

	ImGui::DragFloat("Cut Off", &this->light->getCutOff(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat("Outer Cut Off", &this->light->getOuterCutOff(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f");

	ImGui::End();

	this->position = this->light->getPosition();
	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
}

Core::GameObject* Core::SpotLight::clone()
{
	return new SpotLight(*this);
}

std::string& Core::SpotLight::getType()
{
	return this->type;
}

void Core::SpotLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_SPOT_LIGHT);
	GameObject::serialize(writer);

	writer.Key("Direction");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getDirection().X());
	writer.Double(this->light->getDirection().Y());
	writer.Double(this->light->getDirection().Z());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Diffuse");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getDiffuse().X());
	writer.Double(this->light->getDiffuse().Y());
	writer.Double(this->light->getDiffuse().Z());
	writer.Double(this->light->getDiffuse().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Ambient");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getAmbient().X());
	writer.Double(this->light->getAmbient().Y());
	writer.Double(this->light->getAmbient().Z());
	writer.Double(this->light->getAmbient().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Specular");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->light->getSpecular().X());
	writer.Double(this->light->getSpecular().Y());
	writer.Double(this->light->getSpecular().Z());
	writer.Double(this->light->getSpecular().W());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Constant");
	writer.Double(this->light->getConstant());

	writer.Key("Linear");
	writer.Double(this->light->getLinear());

	writer.Key("Quadratic");
	writer.Double(this->light->getQuadratic());

	writer.Key("CutOff");
	writer.Double(this->light->getCutOff());

	writer.Key("OuterCutOff");
	writer.Double(this->light->getOuterCutOff());

	writer.EndObject();
}

void Core::SpotLight::setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale)
{
	this->position = this->light->getPosition() = position;
}