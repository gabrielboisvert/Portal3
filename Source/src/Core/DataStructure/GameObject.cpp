#include "../../../include/Core/DataStructure/GameObject.h"
#include "../../../include/Application.h"
#include "LibMaths/Mat3/Mat3.h"

int Core::GameObject::uniqueId = 0;

Core::GameObject::GameObject(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform) : name(name), v(&cam.getPosition()), vp(&cam.getVP()), model(model), shader(shader), texture(texture), local(localTransform), global(localTransform)
{
	this->id = std::to_string(Core::GameObject::uniqueId++);
}

Core::GameObject::GameObject(Core::GameObject& obj) : name(obj.name), v(obj.v), vp(obj.vp), model(obj.model), shader(obj.shader), texture(obj.texture), local(obj.local), global(obj.global), position(obj.position), rotation(obj.rotation), scale(obj.scale)
{
	this->id = std::to_string(Core::GameObject::uniqueId++);

	for (std::list<Core::GameObject*>::iterator it = obj.child.begin(); it != obj.child.end(); it++)
		this->addChild(*(**it).clone());

	this->type = obj.type;
}

Core::GameObject::~GameObject()
{
	for (std::list<GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		delete *it;
	this->child.clear();
}

void Core::GameObject::draw()
{
	
	for (std::list<GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		(*it)->draw();

	if (this->shader == nullptr || this->model == nullptr)
		return;

	this->shader->activate(true);
	this->shader->setUniformMat4("model", &global[0][0]);
	this->shader->setUniformMat3("inverseModel", &lm::mat3(global).inverse().transpose()[0][0]);
	this->shader->setUniformVec3("view", &(this->v->X()));
	this->shader->setUniformMat4("mvp", &(*this->vp * global)[0][0]);
	this->shader->setUniform1f("alpha", this->alpha);
	this->shader->setUniform1i("isSelected", Application::getInstance()->getUIManager().getSelectedGameObject() == this);

	if (this->texture != nullptr)
	{
		this->shader->setUniform1i("hasTexture", true);
		this->texture->activate(true);
	}
	else
		this->shader->setUniform1i("hasTexture", false);

	this->model->draw(*this->shader);

	if (this->child.size() == 0)
		return;
}

void Core::GameObject::addChild(GameObject& node)
{
	this->child.push_back(&node);

	if (node.parent != nullptr)
		node.parent->removeChild(node);
	node.parent = this;

	lm::vec3 scale = this->getGlobalScale(*this, lm::vec3(1, 1, 1));

	node.getScale() = lm::vec3(node.getScale().X() / scale.X(), node.getScale().Y() / scale.Y(), node.getScale().Z() / scale.Z());
	node.getPosition() = lm::vec3((node.getPosition().X() - this->position.X()) / scale.X(), (node.getPosition().Y() - this->position.Y()) / scale.Y(), (node.getPosition().Z() - this->position.Z()) / scale.Z());
	node.updateLocal();

	this->updateGlobal();
}

void Core::GameObject::removeChild(GameObject& node)
{
	this->child.remove(&node);
	node.parent = nullptr;

	lm::vec3 scale = this->getGlobalScale(*this, lm::vec3(1, 1, 1));

	node.getScale() = lm::vec3(node.getScale().X() * scale.X(), node.getScale().Y() * scale.Y(), node.getScale().Z() * scale.Z());
	node.getPosition() = lm::vec3( (node.position.X() * scale.X()) + this->position.X(), (node.position.Y() * scale.Y()) + this->position.Y(), (node.position.Z() * scale.Z()) + this->position.Z() );
	node.updateLocal();

	this->updateGlobal();
}

void Core::GameObject::setParent(GameObject& parent)
{
	parent.addChild(*this);
}

void Core::GameObject::setParent(GameObject* parent)
{
	this->parent = parent;
}

Core::GameObject* Core::GameObject::getParent()
{
	return this->parent;
}

Resources::Texture* Core::GameObject::getTexture()
{
	return this->texture;
}

void Core::GameObject::setTexture(Resources::Texture* texture)
{
	this->texture = texture;
}

Resources::Shader* Core::GameObject::getShader()
{
	return this->shader;
}

void Core::GameObject::setShader(Resources::Shader* shader)
{
	this->shader = shader;
}

void Core::GameObject::updateGlobal()
{
	if (this->parent != nullptr)
		this->global = parent->global * this->local;
	else
		this->global = this->local;

	for (std::list<GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		(*it)->updateGlobal();
}

void Core::GameObject::setLocal(const lm::mat4& local)
{
	this->local = local;
	this->updateGlobal();
}

void Core::GameObject::updateLocal()
{
	this->local = lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale);
	this->updateGlobal();
}

std::string& Core::GameObject::getName()
{
	return this->name;
}

const std::string& Core::GameObject::getType()
{
	return this->type;
}

lm::mat4& Core::GameObject::getLocalTransform()
{
	return this->local;
}
lm::mat4& Core::GameObject::getGlobalTransform()
{
	return this->global;
}

lm::mat4& Core::GameObject::getTransform()
{
	return this->global;
}

lm::vec3& Core::GameObject::getPosition()
{
	return this->position;
}

lm::vec3 Core::GameObject::getGlobalPosition()
{
	return lm::vec3(this->global[3][0], this->global[3][1], this->global[3][2]);
}

lm::vec3& Core::GameObject::getRotation()
{
	return this->rotation;
}

lm::mat4 Core::GameObject::getGlobalRotation(const Core::GameObject& obj, lm::mat4& rotation)
{
	if (obj.parent == nullptr)
		return lm::mat4::createTransformMatrix(lm::vec3::zero, obj.rotation, lm::vec3::unitVal) * rotation;
	return getGlobalRotation(*obj.parent, lm::mat4::createTransformMatrix(lm::vec3::zero, obj.rotation, lm::vec3::unitVal) * rotation);
}

lm::vec3& Core::GameObject::getScale()
{
	return this->scale;
}

lm::vec3 Core::GameObject::getGlobalScale(const Core::GameObject& obj, lm::vec3& scale)
{
	if (obj.parent == nullptr)
		return scale.scaled(obj.scale);
	return getGlobalScale(*obj.parent, scale.scaled(obj.scale));
}

Resources::Model* Core::GameObject::getModel()
{
	return this->model;
}

void Core::GameObject::setModel(Resources::Model* model)
{
	this->model = model;
}

float& Core::GameObject::getUniformScale()
{
	return this->scaleUni;
}

void Core::GameObject::setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
}

std::list<Core::GameObject*>& Core::GameObject::getChild()
{
	return this->child;
}

void Core::GameObject::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("Id");
	writer.String(this->id.c_str());

	writer.Key("Name");
	writer.String(this->name.c_str());

	writer.Key("Model");
	writer.String((this->model != nullptr? this->model->getFilename().c_str() : ""));

	writer.Key("Shader");
	writer.String((this->shader != nullptr ? this->shader->getName().c_str() : ""));

	writer.Key("Alpha");
	writer.Double(this->alpha);

	writer.Key("Texture");
	writer.String((this->texture != nullptr? this->texture->getFileName().c_str() : ""));

	writer.Key("Transform");
	writer.StartObject();

	writer.Key("Position");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->position.X());
	writer.Double(this->position.Y());
	writer.Double(this->position.Z());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);


	writer.Key("Rotation");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->rotation.X());
	writer.Double(this->rotation.Y());
	writer.Double(this->rotation.Z());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Scale");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->scale.X());
	writer.Double(this->scale.Y());
	writer.Double(this->scale.Z());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
	writer.EndObject();

	writer.Key("Childs");
	writer.StartArray();
	for (std::list<GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		(*it)->serialize(writer);
	writer.EndArray();
}

void Core::GameObject::update(float deltaTime)
{
	if (deltaTime == 0)
		return;

	this->calculateVectors();
	this->updateLocal();
}

void Core::GameObject::drawUI(GLFWwindow* window)
{
	this->drawRessource(window);
	this->drawTransform(window);
}

void Core::GameObject::drawRessource(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.25f, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.25f, app->getHeight() * 0.25));

	ImGui::Begin("Object Resource", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	bool nodeOpen = ImGui::TreeNodeEx("ID_Model", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, ("Model -> " + (this->model != nullptr ? this->model->getName() : "None")).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("new_object"))
			this->model = app->getUIManager().getSelectedModel();

		ImGui::EndDragDropTarget();
	}
	if (nodeOpen)
		ImGui::TreePop();
	

	nodeOpen = ImGui::TreeNodeEx("ID_shader", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, ("Shader -> " + (this->shader != nullptr ? this->shader->getName() : "None")).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("shader"))
			this->shader = app->getUIManager().getSelectedShader();

		ImGui::EndDragDropTarget();
	}
	if (nodeOpen)
		ImGui::TreePop();

	nodeOpen = ImGui::TreeNodeEx("ID_texture", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, ("Texture -> " + (this->texture != nullptr ? this->texture->getName() : "None")).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("texture"))
			this->texture = app->getUIManager().getSelectedTexture();

		ImGui::EndDragDropTarget();
	}
	if (nodeOpen)
		ImGui::TreePop();

	ImGui::DragFloat("Opacity", &this->alpha, 0.1f, 0, 1, "%.1f");

	ImGui::End();
}

void Core::GameObject::drawTransform(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.25));

	ImGui::Begin("Transform", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	ImGui::DragFloat3("Position", &this->position[0], 0.2f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat3("Rotation", &this->rotation[0], 0.5f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat3("Scale", &this->scale[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f");

	float tmp = this->scaleUni;
	if (ImGui::DragFloat("Uniform Scale", &this->scaleUni, 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
		if (tmp != this->scaleUni)
		{
			this->scale.X() += this->scaleUni - tmp;
			this->scale.Y() += this->scaleUni - tmp;
			this->scale.Z() += this->scaleUni - tmp;
		}

	ImGui::End();

	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
}

void Core::GameObject::calculateVectors()
{
	lm::vec3 front;
	front.X() = sin(rotation.Y() * (M_PI / HALF_CIRCLE)) * cos(rotation.X() * (M_PI / HALF_CIRCLE));
	front.Y() = sin(-rotation.X() * (M_PI / HALF_CIRCLE));
	front.Z() = cos(rotation.X() * (M_PI / HALF_CIRCLE)) * cos(rotation.Y() * (M_PI / HALF_CIRCLE));
	this->forward = front.normalized();

	this->right = this->forward.crossProduct(lm::vec3::up).normalized();
	this->up = this->right.crossProduct(this->forward).normalized();
}

lm::vec3& Core::GameObject::getForward()
{
	return this->forward;
}

lm::vec3& Core::GameObject::getUp()
{
	return this->up;
}

lm::vec3& Core::GameObject::getRight()
{
	return this->right;
}

float& Core::GameObject::getAlpha()
{
	return this->alpha;
}

bool Core::GameObject::countainChild(GameObject* child, bool& flag)
{
	for (std::list<Core::GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
	{
		if (*it == child)
			return flag = true;
		else
			flag = (*it)->countainChild(child, flag);
	}

	return flag;
}

std::string& Core::GameObject::getId()
{
	return this->id;
}
