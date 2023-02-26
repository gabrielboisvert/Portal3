#include "../../../Source/include/Core/DataStructure/PhysicsObject.h"
#include "../../../include/Core/DataStructure/Objects/Player.h"
#include "../../../include/Application.h"

Core::PhysicsObject::PhysicsObject(const std::string& name, Physics::PhysicsEngine& physic, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize) :
	GameObject(name, cam, model, shader, texture, localTransform), physic(&physic)
{
	std::function<void(btRigidBody*, btRigidBody*, void* Physicsobj, btPersistentManifold* persistent)> collisionEnterCallback = [this](btRigidBody* me, btRigidBody* other, void* Physicsobj, btPersistentManifold* persistent) {
		OnCollisionEnter(me, other, (PhysicsObject*)Physicsobj, persistent);
	};

	std::function<void(btRigidBody*, btRigidBody*, void* Physicsobj, btPersistentManifold* persistent)> collisionStayCallback = [this](btRigidBody* me, btRigidBody* other, void* Physicsobj, btPersistentManifold* persistent) {
		OnCollisionStay(me, other, (PhysicsObject*)Physicsobj, persistent);
	};

	std::function<void(btRigidBody*, btRigidBody*, void* Physicsobj, btPersistentManifold* manifold)> collisionExitCallback = [this](btRigidBody* me, btRigidBody* other, void* Physicsobj, btPersistentManifold* manifold) {
		OnCollisionExit(me, other, (PhysicsObject*)Physicsobj, manifold);
	};

	Callbacks.physicsObject = this;
	Callbacks.enter = collisionEnterCallback;
	Callbacks.stay = collisionStayCallback;
	Callbacks.exit = collisionExitCallback;
}

Core::PhysicsObject::PhysicsObject(PhysicsObject& other) : GameObject(other)
{
	this->physic = other.physic;

	std::function<void(btRigidBody*, btRigidBody*, void* Physicsobj, btPersistentManifold* manifold)> collisionEnterCallback = [this](btRigidBody* me, btRigidBody* other, void* Physicsobj, btPersistentManifold* manifold) {
		OnCollisionEnter(me, other, (PhysicsObject*)Physicsobj, manifold);
	};

	std::function<void(btRigidBody*, btRigidBody*, void* Physicsobj, btPersistentManifold* manifold)> collisionStayCallback = [this](btRigidBody* me, btRigidBody* other, void* Physicsobj, btPersistentManifold* manifold) {
		OnCollisionStay(me, other, (PhysicsObject*)Physicsobj, manifold);
	};

	std::function<void(btRigidBody*, btRigidBody*, void* Physicsobj, btPersistentManifold* manifold)> collisionExitCallback = [this](btRigidBody* me, btRigidBody* other, void* Physicsobj, btPersistentManifold* manifold) {
		OnCollisionExit(me, other, (PhysicsObject*)Physicsobj, manifold);
	};

	Callbacks.physicsObject = this;
	Callbacks.enter = collisionEnterCallback;
	Callbacks.stay = collisionStayCallback;
	Callbacks.exit = collisionExitCallback;

	this->rigidBody = new Physics::RigidBody(*other.getRigidBody(), &this->Callbacks);
}

Core::PhysicsObject::~PhysicsObject()
{
	delete this->rigidBody;
}

void Core::PhysicsObject::updateGlobal()
{
	if (this->parent != nullptr)
		this->global = this->parent->getGlobalTransform() * this->local;
	else
		this->global = this->local;

	this->rigidBody->setTransform(this->getGlobalPosition(), this->getGlobalRotation(*this, lm::mat4()), this->getGlobalScale(*this, lm::vec3(1, 1, 1)));
	for (std::list<GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		(*it)->updateGlobal();
}

void Core::PhysicsObject::setLocal(const lm::mat4& local)
{
	this->local = local;
	this->updateGlobal();
}

void Core::PhysicsObject::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	GameObject::serialize(writer);

	writer.Key("RigidBodySize");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(this->rigidBody->getSize().X());
	writer.Double(this->rigidBody->getSize().Y());
	writer.Double(this->rigidBody->getSize().Z());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
}

void Core::PhysicsObject::update(float deltaTime)
{
	if (deltaTime == 0)
		return;

	if (this->parent == nullptr)
		this->position = this->rigidBody->getPosition();
		
	GameObject::update(deltaTime);
}

void Core::PhysicsObject::setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale)
{
	GameObject::setVectorMatrix(position, rotation, scale);
	this->setLocal(lm::mat4::createTransformMatrix(position, rotation, scale));
}

Physics::RigidBody* Core::PhysicsObject::getRigidBody()
{
	return this->rigidBody;
}

void Core::PhysicsObject::OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* persistent)
{
}

void Core::PhysicsObject::OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* persistent)
{
}

void Core::PhysicsObject::OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* persistent)
{
}

void Core::PhysicsObject::drawRessource(GLFWwindow* window)
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

	ImGui::End();
}