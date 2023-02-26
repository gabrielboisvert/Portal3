#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "LowRenderer/Camera.h"
#include "Resources/RessourceManager.h"
#include "LibMaths/Vec2/Vec2.h"
#include "Core/DataStructure/GameObject.h"
#include "Core/DataStructure/Scene.h"
#include "UIManager.h"
#include "Physics/PhysicsEngine.h"
#include "Core/DataStructure/Objects/Player.h"
#include "Core/DataStructure/Objects/Light.h"
#include "Sound/SoundEngine.h"

class Application
{
public:
	enum
	{
		E_PLAY_MODE,
		E_EDITOR_MODE
	};
	struct Delta
	{
		float deltaTime = 0;
		float deltaScale = 1;
	};


	Application(const unsigned int width, const unsigned int height, const char* title);
	~Application();
	void reset();
	void run();
	void drawPortalPerspective(int portal);
	void updateGame();
	void updateEditor();
	void updateDeltaTime();
	float getDeltaTime();
	unsigned int getWidth();
	unsigned int getHeight();
	std::string& getSceneName();
	Physics::PhysicsEngine& getPhysic();
	Sound::SoundEngine& getSound();
	void setPlayer(Core::Player* player);
	Core::Player* getPlayer();
	Resources::ResourceManager& getResourceManager();
	Core::Scene<Core::GameObject>& getScene();
	LowRenderer::Camera& getCam();
	UIManager& getUIManager();
	void drawGame();
	void drawGameUI();
	static unsigned state;
	GLFWwindow* GetWindow();
	void addLight(Core::GameObject& light);
	void removeLight(Core::GameObject& light);
	void reloadCurrent();
	static Application* getInstance();

	private:
		static Application* app;
		GLFWwindow* window = nullptr;
		float lastFrame = 0;
		
		unsigned int width = 0;
		unsigned int height = 0;

		std::string sceneName = "NewScene";
		LowRenderer::Camera cam;
		Resources::ResourceManager resManager;
		Core::Scene<Core::GameObject> scene;
		UIManager ui;

		Delta delta;
		Core::Player* player = nullptr;
		Physics::PhysicsEngine* physicEngine = nullptr;
		Sound::SoundEngine* soundEngine = nullptr;
		bool debugDrawShape = false;
		std::vector<Core::GameObject*> lights;
		

		void processInput();
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_position(GLFWwindow* window, double x, double y);
		static void mouse_button(GLFWwindow* window, int button, int action, int mods);
		static void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};


