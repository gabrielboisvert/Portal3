#include "../../../Source/include/Core/DataStructure/Objects/Platform.h"
#include "../../../../include/Application.h"

Core::Platform::Platform(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, lm::vec3 pMaxVec, float pDuration, const lm::vec3& rigidBodySize)
	: PhysicsObject(name, physic, cam, model, shader, texture, localTransform), maxVec(pMaxVec), duration(pDuration), window(&window)
{
	this->type = "(PlatForm)";
	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_STATIC,
													lm::vec3(localTransform[3][0], localTransform[3][1], localTransform[3][2]),
													rigidBodySize,
													0, physic, &Callbacks);
}

Core::Platform::Platform(Platform& other) : PhysicsObject(other)
{
	this->minVec = other.minVec;
	this->maxVec = other.maxVec;
	this->duration = other.duration;
	this->time = 0;
	this->movingForward = true;
}

void Core::Platform::reset()
{
	this->position = this->minVec;
	this->movingForward = true;
	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
}

void Core::Platform::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_PLATFORM);
	PhysicsObject::serialize(writer);

	writer.Key("Max");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	writer.Double(maxVec.X());
	writer.Double(maxVec.Y());
	writer.Double(maxVec.Z());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	writer.Key("Duration");
	writer.Double(this->duration);

	writer.EndObject();
}

void Core::Platform::setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale)
{
	this->position = position;
	this->minVec = position;
	this->rotation = rotation;
	this->scale = scale;
}

void Core::Platform::update(float delta)
{
	PhysicsObject::update(delta);

	if (delta == 0)
	{
		this->reset();
		this->time = 0;
		return;
	}

	this->time += delta;
	float t = this->time / this->duration;

	if (this->movingForward)
		this->position = lm::vec3::lerp(this->minVec, this->minVec + this->maxVec, t);
	else
		this->position = lm::vec3::lerp(this->minVec + this->maxVec, this->minVec, t);

	if (this->time > this->duration)
	{
		this->movingForward = !this->movingForward;
		this->time = 0;
	}

	
	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
	for (std::list<Core::GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		(*it)->update(delta);
}

Core::GameObject* Core::Platform::clone()
{
	return new Core::Platform(*this);
}

void Core::Platform::drawTransform(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.5f, app->getHeight() * 0.25));

	ImGui::Begin("Transform", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	if (ImGui::DragFloat3("Position", &this->minVec[0], 0.2f, -FLT_MAX, FLT_MAX, "%.3f"))
		this->position = this->minVec;

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

	ImGui::DragFloat3("Unit of Displacement", &this->maxVec[0], 0.2f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat("Duration", &this->duration, 0.1f, -FLT_MAX, FLT_MAX, "%.3f sec");

	ImGui::End();

	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
}

const std::string& Core::Platform::getType()
{
	return this->type;
}

void Core::Platform::OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionEnter(me, other, otherphysicsobj, manifold);

	Core::InteractibleObject* obj = dynamic_cast<Core::InteractibleObject*>(otherphysicsobj);
	if (obj != nullptr)
	{
		btVector3 normal = manifold->getContactPoint(0).m_normalWorldOnB;
		if (normal.y() > 0.7f || normal.y() < -0.7f)
		{
			Core::Player* player = dynamic_cast<Core::Player*>(otherphysicsobj);
			if (player != nullptr)
			{
				if (normal.y() < -0.7f)
					return;

				player->hasJumped() = false;
			}

			this->addChild(*obj);

			Application* app = (Application*)glfwGetWindowUserPointer(this->window);
			app->getScene().removeNode(*obj);
		}
	}
}

void Core::Platform::OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifolds)
{
	PhysicsObject::OnCollisionExit(me, other, otherphysicsobj, manifolds);

	Core::InteractibleObject* obj = dynamic_cast<Core::InteractibleObject*>(otherphysicsobj);
	if (obj != nullptr)
	{
		if (std::find(this->child.begin(), this->child.end(), obj) == this->child.end())
			return;

		Application* app = (Application*)glfwGetWindowUserPointer(this->window);
		if (std::find(app->getPlayer()->getChild().begin(), app->getPlayer()->getChild().end(), obj) == app->getPlayer()->getChild().end())
		{
			app->getScene().addNode(obj);
			this->removeChild(*obj);
		}
		else
			this->child.remove(obj);
	}
}
