#include "../../../Source/include/Core/DataStructure/Objects/Portal.h"
#include <cmath>
#include "../../../../include/Core/Debug/Log.h"
Core::Portal::Portal(const std::string& name, Physics::PhysicsEngine& physic, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* textureConnected, Resources::Texture* textureNotConnected, const lm::mat4& localTransform)
	: PhysicsObject(name, physic, cam, model, shader, nullptr, localTransform), textureConnected(textureConnected), textureNotConnected(textureNotConnected), frame(localTransform * lm::mat4::translation(lm::vec3(0, 0, 0.1f)) * lm::mat4::scale(lm::vec3(1.1f, 1.1f, 1)))
{
	this->position = lm::vec3(this->local[3][0], this->local[3][1], this->local[3][2]);
	this->scale = lm::vec3(localTransform[0][0], localTransform[1][1], localTransform[2][2]);
	this->rigidBody = new Physics::RigidBody(Physics::RigidBody::TypeRigidBody::E_STATIC,
											this->position,
											lm::vec3::unitVal,
											0, physic, &Callbacks);
	physic.getWorld()->removeRigidBody(this->rigidBody->getRigidBody());
}

Core::Portal::~Portal()
{
}

void Core::Portal::draw()
{
	if (!this->isActif || !this->isLinked)
		return;

	this->texture = this->textureNotConnected;

	this->shader->activate(true);
	this->shader->setUniformMat4("mvp", &(*this->vp * this->global)[0][0]);
	this->texture->activate(true);
	this->model->draw(*this->shader);
}

void Core::Portal::drawFrame()
{
	if (!this->isActif)
		return;

	this->shader->activate(true);
	this->shader->setUniformMat4("mvp", &(*this->vp * frame)[0][0]);
	
	if (this->isLinked)
		this->texture = this->textureConnected;
	else
		this->texture = this->textureNotConnected;
	
	this->texture->activate(true);
	this->model->draw(*this->shader);
}

bool& Core::Portal::getIsActif()
{
	return this->isActif;
}

bool& Core::Portal::getIsLinked()
{
	return this->isLinked;
}

void Core::Portal::setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale)
{
	if (rotation.X() >= 90 || rotation.X() <= -90)
		rotation.Z() += 180;

	GameObject::setVectorMatrix(position, rotation, scale);
	this->updateLocal();
	this->frame = this->local * lm::mat4::translation(lm::vec3(0, 0, 0.1f)) * lm::mat4::scale(lm::vec3(1.1f, 1.1f, 1));
}

void Core::Portal::updateGlobal()
{
	if (this->parent != nullptr)
		this->global = this->parent->getGlobalTransform() * this->local;
	else
		this->global = this->local;

	this->rigidBody->setTransform(this->getGlobalPosition(), this->getGlobalRotation(*this, lm::mat4()), this->getGlobalScale(*this, lm::vec3(1, 1, 1)));
	
	if (this->isLinked)
		this->rigidBody->getRigidBody()->setRestitution(1);
	else
		this->rigidBody->getRigidBody()->setRestitution(0);
	for (std::list<GameObject*>::iterator it = this->child.begin(); it != this->child.end(); it++)
		(*it)->updateGlobal();
}

void Core::Portal::rotate(const lm::vec3& normal, float& yaw)
{
	this->rotation.Y() = 90 - yaw;
	this->updateLocal();
	this->frame = this->local * lm::mat4::translation(lm::vec3(0, 0, 0.1f)) * lm::mat4::scale(lm::vec3(1.1f, 1.1f, 1));
}

lm::mat4& Core::Portal::getPortalView()
{
	return this->portalView;
}

void Core::Portal::OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionEnter(me, other, otherphysicsobj, manifold);
}

void Core::Portal::OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionStay(me, other, otherphysicsobj, manifold);
}

void Core::Portal::OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold)
{
	PhysicsObject::OnCollisionExit(me, other, otherphysicsobj);
}

lm::vec3 Core::Portal::getNormal()
{
	return this->forward;
}
