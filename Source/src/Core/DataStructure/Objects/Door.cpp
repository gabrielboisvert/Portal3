#include "../../Source/include/Core/DataStructure/Objects/Door.h"
#include "../../../../include/Application.h"

Core::Door::Door(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize)
	: PhysicsObject(name, physic, cam, model, shader, texture, localTransform), window(&window)
{
	this->type = "(Door)";
	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_STATIC,
													lm::vec3(localTransform[3][0], localTransform[3][1], localTransform[3][2]),
													rigidBodySize,
													0, physic, &Callbacks);
}

Core::Door::Door(Door& other) : PhysicsObject(other)
{
	this->window = other.window;

	// Dont copy other button
	//for (std::list<Core::Button*>::iterator it = other.buttons.begin(); it != other.buttons.end(); it++)
	//	this->addButton(**it);

	this->isOpen = other.isOpen;
}

Core::Door::~Door()
{
	for (std::list<Core::Button*>::iterator it = this->buttons.begin(); it != this->buttons.end(); it++)
		(*it)->removeDoor(*this);
	this->buttons.clear();
}

void Core::Door::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_DOOR);
	PhysicsObject::serialize(writer);

	writer.Key("Opened");
	writer.Bool(this->isOpen);

	writer.Key("Buttons");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	for (std::list<Core::Button*>::iterator it = this->buttons.begin(); it != this->buttons.end(); it++)
		writer.String((*it)->getId().c_str());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.EndObject();
}

Core::GameObject* Core::Door::clone()
{
	return new Core::Door(*this);
}

void Core::Door::addButton(Core::Button& button)
{
	if (std::find(this->buttons.begin(), this->buttons.end(), &button) != this->buttons.end())
		return;

	this->buttons.push_back(&button);
	button.addDoor(*this);
}

void Core::Door::removeButton(Core::Button& button)
{
	this->buttons.remove(&button);
}

void Core::Door::draw()
{
	if (this->isOpen)
		return;

	GameObject::draw();
}

void Core::Door::update(float delta)
{
	PhysicsObject::update(delta);

	if (delta == 0)
		return;

	bool allPress = true;
	for (std::list<Button*>::iterator it = this->buttons.begin(); it != this->buttons.end(); it++)
		if (!(*it)->isPressed())
			allPress = false;

	if (this->isOpen != allPress)
	{
		Application* app = (Application*)glfwGetWindowUserPointer(this->window);
		if (allPress)
		{
			app->getSound().playSound3D("Sounds/SoundEffect/Door_open.mp3", this->getGlobalPosition());
			app->getPhysic().getWorld()->removeRigidBody(this->rigidBody->getRigidBody());
		}
		else
		{
			app->getSound().playSound3D("Sounds/SoundEffect/Door_close.mp3", this->getGlobalPosition());
			app->getPhysic().getWorld()->addRigidBody(this->rigidBody->getRigidBody(), Physics::CollisionGroups::Default, btBroadphaseProxy::AllFilter);
		}
	}
		
	this->isOpen = allPress;
}

void Core::Door::drawRessource(GLFWwindow* window)
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
	ImGui::DragFloat3("RigidBody size", &this->rigidBody->getSize()[0], 0.2f, -FLT_MAX, FLT_MAX, "%.3f");

	nodeOpen = ImGui::TreeNodeEx("Buttons", ImGuiTreeNodeFlags_FramePadding | (this->buttons.size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0), "buttons");
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("object"))
		{
			GameObject* obj = app->getUIManager().getDraggedGameObject();
			if (dynamic_cast<Button*>(obj) != nullptr)
				this->addButton(*(Button*)obj);
		}

		ImGui::EndDragDropTarget();
	}

	if (nodeOpen)
	{
		for (std::list<Button*>::iterator it = this->buttons.begin(); it != this->buttons.end(); it++)
		{
			ImGui::TreeNodeEx(((*it)->getName() + (*it)->getId()).c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, (*it)->getName().c_str());
			if (ImGui::BeginPopupContextItem("RightClickContext"))
			{
				if (ImGui::MenuItem("remove"))
				{
					this->buttons.remove(*it);
					ImGui::EndPopup();
					ImGui::TreePop();
					break;
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}
	
	if (nodeOpen)
		ImGui::TreePop();

	ImGui::End();
}

void Core::Door::drawTransform(GLFWwindow* window)
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

	ImGui::Checkbox("Door opened", &this->isOpen);
	ImGui::End();

	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
}

const std::string& Core::Door::getType()
{
	return this->type;
}