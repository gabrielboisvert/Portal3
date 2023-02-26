#include "../../Source/include/Core/DataStructure/Objects/Button.h"
#include "../../Source/include/Core/DataStructure/Objects/Door.h"


Core::Button::Button(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize)
	: PhysicsObject(name, physic, cam, model, shader, texture, localTransform, rigidBodySize), window(&window)
{
	this->type = "(Button)";
	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_STATIC,
												lm::vec3(localTransform[3][0], localTransform[3][1], localTransform[3][2]),
												rigidBodySize,
												0, physic, &Callbacks);
}

Core::Button::Button(Core::Button& other) : PhysicsObject(other)
{
	this->window = other.window;
	for (std::list<Core::GameObject*>::iterator it = other.linkedDoor.begin(); it != other.linkedDoor.end(); it++)
		this->addDoor(**it);
	this->pressed = other.pressed;
}

Core::Button::~Button()
{
	for (std::list<Core::GameObject*>::iterator it = this->linkedDoor.begin(); it != this->linkedDoor.end(); it++)
		((Core::Door*)(*it))->removeButton(*this);
	this->linkedDoor.clear();
}

bool Core::Button::isPressed()
{
	return this->pressed;
}

const std::string& Core::Button::getType()
{
	return this->type;
}

void Core::Button::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_BUTTON);
	PhysicsObject::serialize(writer);
	writer.EndObject();
}

Core::GameObject* Core::Button::clone()
{
	return new Core::Button(*this);
}

void Core::Button::addDoor(Core::GameObject& door)
{
	if (std::find(this->linkedDoor.begin(), this->linkedDoor.end(), &door) != this->linkedDoor.end())
		return;

	this->linkedDoor.push_back(&door);
	((Core::Door*)&door)->addButton(*this);
}

void Core::Button::removeDoor(Core::GameObject& door)
{
	this->linkedDoor.remove(&door);
}

void Core::Button::OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionEnter(me, other, otherphysicsobj);
	
	if (dynamic_cast<InteractibleObject*>(otherphysicsobj) != nullptr && !this->pressed)
	{
		this->colisionCount++;
		btVector3 normal = manifold->getContactPoint(0).m_normalWorldOnB;
		if (normal.y() < -0.7f || normal.y() > 0.7f)
		{
			if (this->colisionCount == 1)
			{
				Application* app = (Application*)glfwGetWindowUserPointer(this->window);
				app->getSound().playSound3D("Sounds/SoundEffect/Button_sound.mp3", this->getGlobalPosition());
			}
			this->pressed = true;
		}
	}
}

void Core::Button::OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionExit(me, other, otherphysicsobj);
	
	if (dynamic_cast<InteractibleObject*>(otherphysicsobj) != nullptr)
	{
		this->colisionCount = this->colisionCount - 1 < 0? 0: this->colisionCount - 1;
		if (this->colisionCount == 0)
			this->pressed = false;
	}
}
