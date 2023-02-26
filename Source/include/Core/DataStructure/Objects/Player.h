#pragma once
#include "../../../Core/DataStructure/InteractibleObject.h"
#include "Portal.h"

namespace Core
{
	class Reticule
	{
	private:
		enum
		{
			E_FULL,
			E_HALF_BLUE,
			E_HALF_ORANGE,
			E_EMPTY
		};

		Resources::Model* model = nullptr;
		Resources::Shader* shader = nullptr;
		Resources::Texture* textures[4] { nullptr, nullptr, nullptr, nullptr };
		Resources::Texture* current = textures[0];

		lm::mat4 crosshair = lm::mat4::createTransformMatrix(lm::vec3::zero, lm::vec3(0, 0, 0), lm::vec3(0.05f, 0.1f, 1));

	public:
		Reticule(GLFWwindow* window);
		bool left = true;
		bool right = true;
		void draw();
		~Reticule();

	};

	class Gun
	{
	public:
		Gun(lm::mat4& projection, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture);
		void draw();
		~Gun();

	private:
		Resources::Model* model = nullptr;
		Resources::Shader* shader = nullptr;
		Resources::Texture* texture = nullptr;

		lm::mat4 crosshair = lm::mat4::createTransformMatrix(lm::vec3(0.25f, -0.15f, -0.35f), lm::vec3(0, -15, 0), lm::vec3(1.5f, 1.25f, 0.6f));
		lm::mat4& projection;
	};

	class Player : public InteractibleObject
	{
	public:
		enum
		{
			E_LEFT_PORTAL,
			E_RIGHT_PORTAL
		};

		Player(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, float grabRange = 20, float FireRate = 150, float jumpForce = 600, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		~Player();
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		GameObject* clone() override;
		const std::string& getType() override;
		void update(float deltaTime) override;
		void updateGlobal() override;
		void draw() override;
		void shoot(int mouseButton);
		void cancelPortal();
		lm::mat4 portalView(LowRenderer::Camera& cam, int src, int dist);
		void drawPortals();
		void drawPortal(int portal);
		void drawPortalsFrame();
		Core::Portal* getPortal(int portal);
		virtual void drawTransform(GLFWwindow* window) override;
		void OnCollisionEnter(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void OnCollisionStay(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void OnCollisionExit(btRigidBody* me, btRigidBody* other, PhysicsObject* otherphysicsobj, btPersistentManifold* manifold) override;
		void grab();
		void drop();
		bool havingCube();
		void jump();
		void move(float& deltaTime);
		void groundedCheck();
		void teleport(Portal& other) override;
		int& getMovingForward();
		int& getMovingRight();
		Gun* getGun();
		void updateCam();
		bool& hasJumped() { return this->hasJump; };

	private:
		void fixNormal(float& value);

		Reticule* reticule;
		Gun* gun = nullptr;
		Core::Portal* portals[2] = {nullptr, nullptr};
		LowRenderer::Camera& cam;
		bool hasCube = false;
		float grabRange;
		float fireRange;
		float jumpForce;
		bool inTheAir = false;
		bool hasJump = false;
		
		int moveForward = 0;
		int moveRight = 0;
	};
}