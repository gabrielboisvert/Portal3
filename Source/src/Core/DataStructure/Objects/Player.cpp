#include "../../Source/include/Core/DataStructure/Objects/Player.h"
#include "../../../../include/Application.h"
#include "../../Source/include/Core/DataStructure/Objects/CompanionCube.h"
#include "../../Source/include/Physics/RigidBody.h"
#include "../../../../include/Core/DataStructure/Objects/Floor.h"
#include "../../../../include/Core/Debug/Log.h"
#include "../../../../include/Core/DataStructure/Objects/Turret.h"

Core::Player::Player(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, float grabRange, float fireRange, float jumpForce, const lm::vec3& rigidBodySize)
	: InteractibleObject(name, physic, window, cam, model, shader, texture, localTransform, rigidBodySize), cam(cam), grabRange(grabRange), fireRange(fireRange), jumpForce(jumpForce)
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);

	this->reticule = new Reticule(this->window);

	this->gun = new Gun(cam.getProjection(),
						app->getResourceManager().create<Resources::Model>("assets/portal_gun/PortalGun.fbx"),
						app->getResourceManager().create<Resources::Shader>("PlayerUI", "Source/shaders/Vertex/CrosshairVertex.glsl", "Source/Shaders/Fragment/CrosshairFragment.glsl"),
						app->getResourceManager().create<Resources::Texture>("assets/portal_gun/textures/gun.png"));

	Resources::Model* portalModel = app->getResourceManager().create<Resources::Model>("assets/Quad.obj");

	Resources::Texture* bluePortalC = app->getResourceManager().create<Resources::Texture>("assets/Portal/BluePortalConnected.png");
	Resources::Texture* bluePortalNC = app->getResourceManager().create<Resources::Texture>("assets/Portal/BluePortalNotConnected.png");

	Resources::Texture* orangePortalC = app->getResourceManager().create<Resources::Texture>("assets/Portal/OrangePortalConnected.png");
	Resources::Texture* orangePortalNC = app->getResourceManager().create<Resources::Texture>("assets/Portal/OrangePortalNotConnected.png");

	Resources::Shader* sh = app->getResourceManager().create<Resources::Shader>("Portal", "Source/shaders/Vertex/UnlitVertex.glsl", "Source/shaders/Fragment/PortalFragment.glsl");

	this->portals[E_LEFT_PORTAL] = new Portal("PortalBlue", physic, cam, portalModel, sh, bluePortalC, bluePortalNC, lm::mat4::createTransformMatrix(lm::vec3::zero, lm::vec3::zero, lm::vec3(4, 7, 0.3f)));
	this->portals[E_RIGHT_PORTAL] = new Portal("PortalOrange", physic, cam, portalModel, sh, orangePortalC, orangePortalNC, lm::mat4::createTransformMatrix(lm::vec3::zero, lm::vec3::zero, lm::vec3(4, 7, 0.3f)));

	this->type = "(Player)";
	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_DYNAMIC,
											cam.getInGamePosition(),
											rigidBodySize,
											30, physic, &Callbacks, true);
	this->rigidBody->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
}

Core::Player::~Player()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	app->setPlayer(nullptr);
	app->getScene().removeNode(*this);

	delete this->gun;
	delete this->portals[0];
	delete this->portals[1];
}

void Core::Player::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_PLAYER);
	InteractibleObject::serialize(writer);

	writer.Key("Grab range");
	writer.Double(this->grabRange);

	writer.Key("Fire range");
	writer.Double(this->fireRange);

	writer.Key("Jump force");
	writer.Double(this->jumpForce);

	writer.EndObject();
}

Core::GameObject* Core::Player::clone()
{
	return new Core::Player(*this);
}

const std::string& Core::Player::getType()
{
	return this->type;
}

void Core::Player::drawTransform(GLFWwindow* window)
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

	ImGui::DragFloat("Grab range", &this->grabRange, 0.5f, -FLT_MAX, FLT_MAX, "%.1f");
	ImGui::DragFloat("Fire range", &this->fireRange, 0.5f, -FLT_MAX, FLT_MAX, "%.1f");
	ImGui::DragFloat("Jump force", &this->jumpForce, 0.5f, -FLT_MAX, FLT_MAX, "%.1f");

	ImGui::End();
	if (app->getDeltaTime() != 0)
		cam.getInGamePosition() = this->position;
	
	this->local = lm::mat4::createTransformMatrix(this->position, this->rotation, this->scale);
	this->updateGlobal();
}

void Core::Player::draw()
{
	GameObject::draw();
	this->gun->draw();

	this->reticule->draw();
}

void Core::Player::drawPortalsFrame()
{
	this->portals[E_LEFT_PORTAL]->drawFrame();
	this->portals[E_RIGHT_PORTAL]->drawFrame();
}

Core::Portal* Core::Player::getPortal(int portal)
{
	return this->portals[portal];
}

lm::mat4 Core::Player::portalView(LowRenderer::Camera& cam, int src, int dist)
{
	lm::mat4 mv = cam.getView() * this->portals[src]->getGlobalTransform();
	return mv * lm::mat4::yRotation(180) * this->portals[dist]->getGlobalTransform().inverse();
}

void Core::Player::drawPortals()
{
	this->portals[E_LEFT_PORTAL]->draw();
	this->portals[E_RIGHT_PORTAL]->draw();
}

void Core::Player::drawPortal(int portal)
{
	this->portals[portal]->draw();
}

void Core::Player::shoot(int mouseButton)
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	app->getSound().playSound2D("Sounds/SoundEffect/Gun_Shoot.mp3");

	lm::vec3 globalPos = this->getGlobalPosition();

	btVector3 startPos(
		globalPos.X(),
		globalPos.Y(),
		globalPos.Z()
	);

	btVector3 endPos(
		(globalPos.X() + cam.getInGameForward().X() * this->fireRange),
		(globalPos.Y() + cam.getInGameForward().Y() * this->fireRange),
		(globalPos.Z() + cam.getInGameForward().Z() * this->fireRange)
	);

	btCollisionWorld::ClosestRayResultCallback rayCallback(startPos, endPos);
	rayCallback.m_collisionFilterGroup = Physics::CollisionGroups::Default;
	rayCallback.m_collisionFilterMask = Physics::CollisionGroups::Default;

	try
	{
		physic->getWorld()->rayTest(startPos, endPos, rayCallback);
	}
	catch (...)
	{
		return;
	}
	physic->getWorld()->getDebugDrawer()->drawLine(startPos, endPos, btVector3(0, 1, 1));

	if (rayCallback.hasHit())
	{
		Physics::CollisionCallbacks* callback = (Physics::CollisionCallbacks*)rayCallback.m_collisionObject->getUserPointer();
		if (this->rigidBody->getRigidBody()->getUserPointer() != callback)
		{
			Core::Floor* obj = dynamic_cast<Core::Floor*>((Core::PhysicsObject*)callback->physicsObject);
			if (obj != nullptr)
			{
				Core::Portal* portal = nullptr;
				Core::Portal* otherPortal = nullptr;
				lm::vec3 pos((float)rayCallback.m_hitPointWorld.x(), (float)rayCallback.m_hitPointWorld.y(), (float)rayCallback.m_hitPointWorld.z());
				if (mouseButton == GLFW_MOUSE_BUTTON_LEFT)
				{
					portal = this->portals[E_LEFT_PORTAL];
					otherPortal = this->portals[E_RIGHT_PORTAL];

					if (pos == otherPortal->getPosition() && otherPortal->getIsActif())
						return;

					if (!this->portals[E_LEFT_PORTAL]->getIsActif())
					{
						this->portals[E_LEFT_PORTAL]->getIsActif() = true;
						this->physic->getWorld()->addRigidBody(this->portals[0]->getRigidBody()->getRigidBody(), Physics::CollisionGroups::Default, btBroadphaseProxy::AllFilter);
						this->reticule->left = false;
					}
				}
				else if (mouseButton == GLFW_MOUSE_BUTTON_RIGHT)
				{
					portal = this->portals[E_RIGHT_PORTAL];
					otherPortal = this->portals[E_LEFT_PORTAL];

					if (pos == otherPortal->getPosition() && otherPortal->getIsActif())
						return;

					if (!this->portals[E_RIGHT_PORTAL]->getIsActif())
					{
						this->portals[E_RIGHT_PORTAL]->getIsActif() = true;
						this->physic->getWorld()->addRigidBody(this->portals[1]->getRigidBody()->getRigidBody(), Physics::CollisionGroups::Default, btBroadphaseProxy::AllFilter);
						this->reticule->right = false;
					}
				}
				this->portals[E_LEFT_PORTAL]->getIsLinked() = this->portals[E_RIGHT_PORTAL]->getIsLinked() = this->portals[E_LEFT_PORTAL]->getIsActif() && this->portals[E_RIGHT_PORTAL]->getIsActif();


				lm::vec3 normal((float)rayCallback.m_hitNormalWorld.x(), (float)rayCallback.m_hitNormalWorld.y(), (float)rayCallback.m_hitNormalWorld.z());
				this->fixNormal(normal.X());
				this->fixNormal(normal.Y());
				this->fixNormal(normal.Z());

				lm::vec3 axis = lm::vec3::backward.crossProduct(normal).normalized();
				float angle = lm::vec3::radiansToDegrees(std::acos(lm::vec3::backward.dotProduct(normal)));

				if (normal.Z() == -1)
				{
					axis.Y() = 1;
					angle = 180;
				}
				
				portal->setVectorMatrix(pos, axis * angle, portal->getScale());

				if (normal.Y() >= 1 || normal.Y() <= -1)
					portal->rotate(normal, this->cam.getInGameRotationYaw());
			}
		}
	}
}

void Core::Player::cancelPortal()
{
	this->portals[E_LEFT_PORTAL]->getIsActif() = false;
	this->portals[E_LEFT_PORTAL]->getIsLinked() = false;

	this->portals[E_RIGHT_PORTAL]->getIsActif() = false;
	this->portals[E_RIGHT_PORTAL]->getIsLinked() = false;

	this->physic->getWorld()->removeRigidBody(this->portals[0]->getRigidBody()->getRigidBody());
	this->physic->getWorld()->removeRigidBody(this->portals[1]->getRigidBody()->getRigidBody());

	this->reticule->left = true;
	this->reticule->right = true;
}

Core::Reticule::Reticule(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	this->model = app->getResourceManager().create<Resources::Model>("assets/Quad.obj");
	
	this->shader = app->getResourceManager().create<Resources::Shader>("PlayerUI", "Source/shaders/Vertex/CrosshairVertex.glsl", "Source/Shaders/Fragment/CrosshairFragment.glsl");

	this->textures[0] = app->getResourceManager().create<Resources::Texture>("assets/CrossHair/FullCrosshair.png");
	this->textures[1] = app->getResourceManager().create<Resources::Texture>("assets/CrossHair/HalfBlueCrosshair.png");
	this->textures[2] = app->getResourceManager().create<Resources::Texture>("assets/CrossHair/HalfOrangeCrosshair.png");
	this->textures[3] = app->getResourceManager().create<Resources::Texture>("assets/CrossHair/EmptyCrosshair.png");
}

void Core::Reticule::draw()
{
	if (Application::state == Application::E_EDITOR_MODE)
		return;

	if (this->left && this->right)
		this->current = this->textures[E_FULL];
	else if (this->left && !this->right)
		this->current = this->textures[E_HALF_BLUE];
	else if (!this->left && this->right)
		this->current = this->textures[E_HALF_ORANGE];
	else
		this->current = this->textures[E_EMPTY];

	glDisable(GL_DEPTH_TEST);

	this->shader->activate(true);
	this->shader->setUniformMat4("model", &this->crosshair[0][0]);

	this->current->activate(true);
	this->model->draw(*this->shader);

	glEnable(GL_DEPTH_TEST);
}

void Core::Player::grab()
{
	btVector3 startPos(
		cam.getInGamePosition().X(),
		cam.getInGamePosition().Y(),
		cam.getInGamePosition().Z()
	);

	btVector3 endPos(
		(cam.getInGamePosition().X() + cam.getInGameForward().X() * this->grabRange),
		(cam.getInGamePosition().Y() + cam.getInGameForward().Y() * this->grabRange),
		(cam.getInGamePosition().Z() + cam.getInGameForward().Z() * this->grabRange)
	);

	btCollisionWorld::ClosestRayResultCallback rayCallback(startPos, endPos);
	rayCallback.m_collisionFilterGroup = Physics::CollisionGroups::Default;
	rayCallback.m_collisionFilterMask = Physics::CollisionGroups::Portal;

	try
	{
		physic->getWorld()->rayTest(startPos, endPos, rayCallback);
	}
	catch (...)
	{
		return;
	}
	physic->getWorld()->getDebugDrawer()->drawLine(startPos, endPos, btVector3(0, 1, 1));

	if (rayCallback.hasHit())
	{
		Physics::CollisionCallbacks* callback = (Physics::CollisionCallbacks*)rayCallback.m_collisionObject->getUserPointer();
		if (this->rigidBody->getRigidBody()->getUserPointer() != callback)
		{
			Core::InteractibleObject* obj = dynamic_cast<Core::InteractibleObject*>((Core::PhysicsObject*)callback->physicsObject);
			if (!this->hasCube && obj != nullptr)
			{
				Core::Turret* turret = dynamic_cast<Core::Turret*>(obj);
				if (turret != nullptr)
					turret->kill();

				Application* app = (Application*)glfwGetWindowUserPointer(this->window);
				this->addChild(*obj);

				obj->setLocal(lm::mat4::createTransformMatrix(lm::vec3(10, 0, 0), lm::vec3::zero, obj->getScale()));
				obj->getRigidBody()->setType(Physics::RigidBody::TypeRigidBody::E_KINEMATIC);
				obj->getRigidBody()->getEngine()->getWorld()->removeCollisionObject(obj->getRigidBody()->getRigidBody());

				app->getScene().removeNode(*obj);
				this->hasCube = true;
			}
		}
	}
}

void Core::Player::drop()
{
	if (!this->hasCube)
		return;

	Application* app = (Application*)glfwGetWindowUserPointer(window);

	Core::InteractibleObject* cube = ((Core::InteractibleObject*)this->child.front());
	lm::vec3 pos = cube->getGlobalPosition();

	this->removeChild(*cube);
	
	InteractibleObject* newObj = (InteractibleObject*)cube->clone();
	newObj->setVectorMatrix(pos, this->rotation, cube->getScale());
	app->getScene().addNode(newObj);
	
	this->hasCube = false;
	if (app->getUIManager().getSelectedGameObject() == cube)
		app->getUIManager().setSelectedGameObject(nullptr);

	delete cube;
}

bool Core::Player::havingCube()
{
	return hasCube;
}

void Core::Player::update(float deltaTime)
{
	if (deltaTime == 0)
		return;

	this->move(deltaTime);

	if (this->parent == nullptr)
		this->position = this->rigidBody->getPosition();
	else
	{
		this->position = lm::vec3((this->rigidBody->getPosition().X() - this->parent->getPosition().X()) / this->parent->getScale().X(),
			(this->rigidBody->getPosition().Y() - this->parent->getPosition().Y()) / this->parent->getScale().Y(),
			(this->rigidBody->getPosition().Z() - this->parent->getPosition().Z()) / this->parent->getScale().Z());
	}

	this->portals[0]->update(deltaTime);
	this->portals[1]->update(deltaTime);

	this->rotation = lm::vec3(this->rotation.X(), -cam.getInGameRotationYaw(), this->rotation.Z());
	this->updateLocal();
}

void Core::Player::updateGlobal()
{
	if (this->parent != nullptr)
	{
		this->global = this->parent->getGlobalTransform() * this->local;
		if (this->moveForward == 0 && this->moveRight == 0 && !this->hasJump)
		{
			this->rigidBody->setTransform(this->getGlobalPosition(), this->getGlobalRotation(*this, lm::mat4()), this->getGlobalScale(*this, lm::vec3(1, 1, 1)));
		}
		else
		{
			if (this->rigidBody->getRigidBody()->getLinearVelocity().y() < 0)
				this->hasJump = false;

			lm::vec3 pos = this->rigidBody->getPosition();
			if (!this->hasJump)
				pos.Y() = this->getGlobalPosition().Y();
			this->rigidBody->setTransform(pos, this->getGlobalRotation(*this, lm::mat4()), this->getGlobalScale(*this, lm::vec3(1, 1, 1)));
		}
	}
	else
	{
		this->global = this->local;
		this->rigidBody->setTransform(this->getGlobalPosition(), this->getGlobalRotation(*this, lm::mat4()), this->getGlobalScale(*this, lm::vec3(1, 1, 1)));
	}

	if (this->portals[0]->getIsLinked())
		this->rigidBody->getRigidBody()->setRestitution(0.8f);
	else
		this->rigidBody->getRigidBody()->setRestitution(0);

	for (std::list<GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		(*it)->updateGlobal();
}

void Core::Player::OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionEnter(me, other, otherphysicsobj, manifold);

	Portal* portal = dynamic_cast<Portal*>(otherphysicsobj);
	if (portal != nullptr && portal->getIsLinked())
	{
		this->teleport((otherphysicsobj == this->portals[0] ? *this->portals[1] : *this->portals[0]));
		return;
	}

	btVector3 normal = manifold->getContactPoint(0).m_normalWorldOnB;
}

void Core::Player::OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
}

void Core::Player::teleport(Portal& portal)
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);

	Portal* other = (&portal == this->portals[0] ? this->portals[1] : this->portals[0]);
	
	lm::vec3 portalFoward = portal.getForward();

	if ((portalFoward.Y() != 1 && other->getForward().Y() != 1))
	{
		float xDiff = portal.getRotation().X() - other->getRotation().X();
		float yDiff = portal.getRotation().Y() - other->getRotation().Y();
		cam.getInGameRotationYaw() += 180 - (xDiff + yDiff);
		cam.updateCameraVectors();
	}
	
	btVector3 velo = this->getRigidBody()->getRigidBody()->getLinearVelocity();
	lm::vec3 d(velo.x(), velo.y(), velo.z());
	lm::vec3 r = d.length() * portalFoward;
	float x = r.X();
	float y = r.Y();
	float z = r.Z();
	this->getRigidBody()->getRigidBody()->setLinearVelocity(btVector3(x, y, z));

	lm::vec3 newPos = portal.getPosition();
	this->position = newPos + (portalFoward * 5);

	this->updateLocal();
}

void Core::Player::OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionExit(me, other, otherphysicsobj, manifold);
}

int& Core::Player::getMovingForward()
{
	return this->moveForward;
}

int& Core::Player::getMovingRight()
{
	return this->moveRight;
}

Core::Gun* Core::Player::getGun()
{
	return this->gun;
}

void Core::Player::updateCam()
{
	cam.getInGamePosition() = this->getGlobalPosition();
}

void Core::Player::fixNormal(float& value)
{
	if (value < -0.2f)
		value = -1;
	else if (value > 0.2f)
		value = 1;
	else
		value = 0;
}

void Core::Player::jump()
{
	if (this->inTheAir)
		return;

	this->rigidBody->getRigidBody()->applyCentralImpulse(btVector3(0, this->jumpForce, 0));
	this->hasJump = true;
}

void Core::Player::move(float& deltaTime)
{
	float velocity = cam.getSpeed() * (cam.isRunning() ? (this->inTheAir ? 1: 1.5f) : 1);
	lm::vec3 front = cam.calculateFront() * velocity;
	lm::vec3 right = cam.getInGameRight() * velocity;

	btVector3 moveForward = (btVector3(front.X(), front.Y(), front.Z()) * this->moveForward);
	btVector3 moveRight = (btVector3(right.X(), right.Y(), right.Z()) * this->moveRight);
	
	btVector3 movement(0, 0, 0);

	//Short raycast to check if player is on ground
	this->groundedCheck();

	if (this->inTheAir)
	{
		movement = this->rigidBody->getRigidBody()->getLinearVelocity();
		this->rigidBody->getRigidBody()->applyCentralForce((moveForward + moveRight) * 50);
	}
	else
	{
		this->rigidBody->getRigidBody()->clearForces();
		movement += moveForward;
		movement += moveRight;
	}

	movement[1] = this->rigidBody->getRigidBody()->getLinearVelocity().y() - (this->rigidBody->getRigidBody()->getMass() * deltaTime);
	this->rigidBody->getRigidBody()->setLinearVelocity(movement);
	

	this->getMovingForward() = 0;
	this->getMovingRight() = 0;
}

void Core::Player::groundedCheck()
{
	lm::vec3 pos = this->getGlobalPosition();
	btVector3 startPos(pos.X(), pos.Y(), pos.Z());
	btVector3 endPos(pos.X(), (pos.Y() - (this->rigidBody->getSize().Y() + 0.1f )), pos.Z());

	btCollisionWorld::ClosestRayResultCallback rayCallback(startPos, endPos);
	rayCallback.m_collisionFilterGroup = Physics::CollisionGroups::Default;
	rayCallback.m_collisionFilterMask = Physics::CollisionGroups::Default;
	try
	{
		physic->getWorld()->rayTest(startPos, endPos, rayCallback);
	}
	catch (...)
	{
		return;
	}
	if (rayCallback.hasHit())
	{
		Physics::CollisionCallbacks* callback = (Physics::CollisionCallbacks*)rayCallback.m_collisionObject->getUserPointer();
		if (this->rigidBody->getRigidBody()->getUserPointer() != callback)
			this->inTheAir = false;
	}
	else
		this->inTheAir = true;
}



Core::Reticule::~Reticule()
{
}




Core::Gun::Gun(lm::mat4& projection, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture) : projection(projection), model(model), shader(shader), texture(texture)
{
}

void Core::Gun::draw()
{
	if (Application::state == Application::E_EDITOR_MODE)
		return;

	this->shader->activate(true);
	this->shader->setUniformMat4("model", &(this->projection * this->crosshair)[0][0]);

	this->texture->activate(true);
	this->model->draw(*this->shader);
}

Core::Gun::~Gun()
{
}