#include "../../Source/include/Core/DataStructure/Objects/Turret.h"
#include "../../../../include/Core/Debug/Log.h"

Core::Turret::Turret(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const float& radius, const float& viewAngle, const lm::mat4& localTransform, const lm::vec3& rigidBodySize)
	: InteractibleObject(name, physic, window, cam, model, shader, texture, localTransform, rigidBodySize), radius(radius), viewAngle(viewAngle)
{
	this->type = "(Turret)";
	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_DYNAMIC,
											lm::vec3(localTransform[3][0], localTransform[3][1], localTransform[3][2]),
											rigidBodySize,
											100, physic, &Callbacks);
	this->rigidBody->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->rigidBody->getRigidBody()->applyGravity();
}

Core::Turret::Turret(Turret& other) : InteractibleObject(other)
{
	this->window = other.window;
	this->isAlive = other.isAlive;
	this->deathTime = other.deathTime;
	this->radius = other.radius;
	this->viewAngle = other.viewAngle;
	this->rigidBody->setType(Physics::RigidBody::TypeRigidBody::E_DYNAMIC);
	this->rigidBody->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->rigidBody->getRigidBody()->applyGravity();
}

void Core::Turret::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_TURRET);
	InteractibleObject::serialize(writer);

	writer.Key("Radius");
	writer.Double(this->radius);

	writer.Key("ViewAngle");
	writer.Double(this->viewAngle);

	writer.EndObject();
}

Core::GameObject* Core::Turret::clone()
{
	return new Core::Turret(*this);
}

const std::string& Core::Turret::getType()
{
	return this->type;
}

void Core::Turret::OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	InteractibleObject::OnCollisionEnter(me, other, otherphysicsobj, manifold);
	Core::Player* player = dynamic_cast<Player*>(otherphysicsobj);
	if (player != nullptr)
	{
		if (this->rotation.Z() == 0)
		{
			this->deathDir = this->rotation + -lm::vec3(manifold->getContactPoint(0).m_normalWorldOnB.x(), manifold->getContactPoint(0).m_normalWorldOnB.y(), manifold->getContactPoint(0).m_normalWorldOnB.z()).crossProduct(lm::vec3::up) * 90;
			this->deathTime = 0;
		}
		
		this->kill();
	}
}

void Core::Turret::OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionExit(me, other, otherphysicsobj, manifold);
}

void Core::Turret::kill()
{
	if (this->isAlive == false)
		return;

	this->isAlive = false;
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	app->getSound().playSound3D("Sounds/SoundEffect/Turret_death.mp3", this->getGlobalPosition());
}

void Core::Turret::update(float deltaTime)
{	
	this->calculateVectors();

	if (this->isAlive)
		this->raycasting();

	if (deltaTime == 0)
		return;

	PhysicsObject::update(deltaTime);
	if (!this->isAlive)
	{
		this->deathTime += deltaTime;
		float t = this->deathTime / 2;

		if (t > 1)
			return;

		this->rotation = lm::vec3::lerp(this->rotation, this->deathDir, t);
		return;
	}
	

	if (this->soundEffect != nullptr)
	{
		if (this->soundEffect->isFinished())
		{
			this->soundEffect->drop();
			Application* app = (Application*)glfwGetWindowUserPointer(this->window);
			app->reloadCurrent();
		}
	}
}

void Core::Turret::raycasting()
{
	btVector3 startPos(
		this->position.X() + this->forward.X(),
		this->position.Y() + this->forward.Y(),
		this->position.Z() + this->forward.Z()
	);

	btVector3 endPos(
		this->position.X() + this->forward.X() * radius,
		this->position.Y() + this->forward.Y() * radius,
		this->position.Z() + this->forward.Z() * radius
	);
	physic->getWorld()->getDebugDrawer()->drawLine(startPos, endPos, btVector3(1, 0, 0));

	Player* player = ((Application*)glfwGetWindowUserPointer(this->window))->getPlayer();
	if (player == nullptr || this->position.distance(player->getPosition()) > this->radius || (player->getPosition() - this->position).degAngle(this->forward) > this->viewAngle)
	{
		this->time = 0;
		this->hasViewPlayer = false;
		return;
	}


	btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(this->position.X(), this->position.Y(), this->position.Z()), btVector3(player->getPosition().X(), player->getPosition().Y(), player->getPosition().Z()));
	RayCallback.m_collisionFilterGroup = Physics::CollisionGroups::Default;
	RayCallback.m_collisionFilterMask = -1;

	try
	{
		physic->getWorld()->rayTest(btVector3(this->position.X(), this->position.Y(), this->position.Z()), btVector3(player->getPosition().X(), player->getPosition().Y(), player->getPosition().Z()), RayCallback);
	}
	catch (...)
	{
		return;
	}

	if (RayCallback.m_collisionObject == nullptr)
		return;

	if (!RayCallback.hasHit())
	{
		this->time = 0;
		this->hasViewPlayer = false;
		return;
	}

	Physics::CollisionCallbacks* Callbacks = (Physics::CollisionCallbacks*)RayCallback.m_collisionObject->getUserPointer();
	PhysicsObject* obj = (PhysicsObject*)Callbacks->physicsObject;

	if (dynamic_cast<Player*>(obj) == nullptr)
	{
		this->time = 0;
		this->hasViewPlayer = false;
		return;
	}


	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	if (!this->hasViewPlayer)
	{
		app->getSound().playSound3D("Sounds/SoundEffect/Turret_view_player.mp3", this->getGlobalPosition());
		this->hasViewPlayer = true;
	}
	

	this->shoot(app->getDeltaTime());
}

void Core::Turret::shoot(float deltaTime)
{
	this->time += deltaTime;

	if (this->time < this->fireRate)
		return;

	if (this->soundEffect == nullptr)
	{
		Application* app = (Application*)glfwGetWindowUserPointer(this->window);
		this->soundEffect = app->getSound().playMusic3D("Sounds/SoundEffect/Turret_Shoot.mp3", this->getGlobalPosition());
	}
}

void Core::Turret::setRadius(const float& radius)
{
	this->radius = radius;
}

void Core::Turret::drawTransform(GLFWwindow* window)
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

	ImGui::DragFloat("radius", &this->radius, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
	ImGui::DragFloat("viewAngle", &this->viewAngle, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");

	ImGui::End();
	this->setLocal(lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale));
}