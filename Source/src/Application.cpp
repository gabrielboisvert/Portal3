
#include "../include/Application.h"
#include "../include/SceneLoader.h"
#include "../include/Core/Debug/Log.h"
#include "../include/LowRenderer/Mesh.h"
#include "../include/Resources/Texture.h"

unsigned int Application::state = E_PLAY_MODE;
Application* Application::app = nullptr;
Application* Application::getInstance()
{
	return Application::app;
}

void Application::processInput()
{
	if (this->ui.isRenameWindowActif())
		return;

	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, true);
	
	this->cam.setShift(glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	if (Application::state == E_PLAY_MODE)
	{
		if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
			this->player->getMovingRight() = -1;
		if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
			this->player->getMovingRight() = 1;

		if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
			this->player->getMovingForward() = 1;
		if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
			this->player->getMovingForward() = -1;
	}
	else
	{
		if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
			this->cam.processKeyboard(LowRenderer::Movement::LEFT, this->delta.deltaTime);
		else if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
			this->cam.processKeyboard(LowRenderer::Movement::RIGHT, this->delta.deltaTime);

		if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
			this->cam.processKeyboard(LowRenderer::Movement::FORWARD, this->delta.deltaTime);
		else if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
			this->cam.processKeyboard(LowRenderer::Movement::BACKWARD, this->delta.deltaTime);
	}
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);
	if (app->ui.isRenameWindowActif())
		return;

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
	{
		Application::state = (Application::state == E_EDITOR_MODE ? E_PLAY_MODE : E_EDITOR_MODE);
		if (Application::state == E_PLAY_MODE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, app->cam.getLastX(), app->cam.getLastY());
			app->cam.setFocus(true);
			app->delta.deltaScale = 1;
			
			if (app->player == nullptr)
			{
				app->setPlayer(new Core::Player("Player", *app->physicEngine, *app->window, app->cam, nullptr, nullptr, nullptr, lm::mat4::identity));
				app->scene.addNode(app->player);
			}
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			app->cam.setFocus(false);
			app->delta.deltaScale = 0;
		}

		app->cam.switchCam();
	}

	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
		app->debugDrawShape = !app->debugDrawShape;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		app->player->cancelPortal();

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (app->player->havingCube())
			app->player->drop();
		else
			app->player->grab();
	}

	if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && app->state == E_EDITOR_MODE)
		app->ui.deleteGameObject();

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && app->state == E_EDITOR_MODE)
		app->ui.save();

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && app->state == E_EDITOR_MODE)
		app->ui.newScene();

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && app->state == E_EDITOR_MODE)
		app->ui.copy();

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && app->state == E_EDITOR_MODE)
		app->ui.paste();

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		app->player->jump();
}

GLFWwindow* Application::GetWindow()
{
	return window;
}

void Application::addLight(Core::GameObject& light)
{
	this->lights.push_back(&light);
}

void Application::removeLight(Core::GameObject& light)
{
	std::vector<Core::GameObject*>::iterator it = std::find(this->lights.begin(), this->lights.end(), &light);
	if (it != this->lights.end())
		this->lights.erase(it);
}

void Application::reloadCurrent()
{
	this->ui.loadScene(this->sceneName);
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Application* app = (Application*)glfwGetWindowUserPointer(window);
	app->width = width;
	app->height = height;
	app->cam.changeViewPortSize(lm::mat4::perspectiveProjection(90, float(width) / float(height), 0.1f, 500.f));
}

void Application::mouse_position(GLFWwindow* window, double x, double y)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);
	app->cam.setMousePosition(x, y);
}

void Application::mouse_button(GLFWwindow* window, int button, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);
	if (((button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) || (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)) && Application::state == Application::E_PLAY_MODE)
		app->player->shoot(button);
}

Application::Application(const unsigned int width, const unsigned int height, const char* title) : width(width), height(height), cam(LowRenderer::Camera(width, height, lm::mat4::perspectiveProjection(90, float(width) / float(height), 0.1f, 500.f), lm::vec3(0, 8, 8)))
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	this->window = glfwCreateWindow(width, height, title, NULL, NULL);

	// Center window
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	if (primary)
	{
		const GLFWvidmode* desktop = glfwGetVideoMode(primary);
		glfwSetWindowPos(window, (desktop->width - width) / 2, (desktop->height - height) / 2);
	}

	if (window == NULL)
	{
		glfwTerminate();
		Core::Log::print("Failed to open window");
		exit(EXIT_FAILURE);
	}
	
	glfwMakeContextCurrent(this->window);
	glfwSetFramebufferSizeCallback(this->window, Application::framebuffer_size_callback);

	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(this->window, Application::mouse_position);
	glfwSetMouseButtonCallback(this->window, Application::mouse_button);
	glfwSetKeyCallback(this->window, Application::key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Core::Log::print("Failed to initialize GLAD");
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(this->window, this);
	
	this->physicEngine = new Physics::PhysicsEngine(this->window);
	this->soundEngine = new Sound::SoundEngine();

	this->ui.init(this->window);
	
	if (this->player == nullptr)
	{
		this->player = new Core::Player("player", *this->physicEngine, *this->window, this->cam, nullptr, nullptr, nullptr, lm::mat4::identity);
		this->scene.addNode(this->player);
	}

	Application::app = this;
}

void Application::run()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	
	glClearColor(0.0f, 0.6f, 0.60f, 1.0f);
	this->updateDeltaTime();
	this->cam.updatePos();
	while (!glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();

		this->updateDeltaTime();
		this->updateGame();
		this->processInput();
		this->cam.updatePos();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);

		// Draw to stencil buffer
		glEnable(GL_STENCIL_TEST);
		this->drawPortalPerspective(Core::Player::E_LEFT_PORTAL);
		this->drawPortalPerspective(Core::Player::E_RIGHT_PORTAL);
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// Draw to depth buffera
		glClear(GL_DEPTH_BUFFER_BIT);
		if (this->player != nullptr)
			this->player->drawPortals();
		
		// Draw to color buffer
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDisable(GL_CULL_FACE);
		this->drawGame();
		this->drawGameUI();
		
		//Draw physic Shape
		if (this->debugDrawShape)
			this->physicEngine->getWorld()->debugDrawWorld();

		// Draw ui
		if (this->state == E_EDITOR_MODE)
			this->updateEditor();
		
		glfwSwapBuffers(this->window);
	}
}

void Application::drawPortalPerspective(int portal)
{
	if (this->player == nullptr || !this->player->getPortal(portal)->getIsLinked())
		return;

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF);

	glClear(GL_STENCIL_BUFFER_BIT);

	this->player->drawPortal(portal);
	// Compute new world camera position
	lm::mat4 v = this->player->portalView(this->cam, portal, (portal == Core::Player::E_LEFT_PORTAL? Core::Player::E_RIGHT_PORTAL : Core::Player::E_LEFT_PORTAL));
	
	lm::vec3& a = cam.getPosition();
	lm::vec3& b = player->getPortal(portal)->getPosition();
	float dist = sqrt(((a.X() - b.X()) * (a.X() - b.X())) + ((a.Y() - b.Y()) * (a.Y() - b.Y())) + ((a.Z() - b.Z()) * (a.Z() - b.Z())) );

	// To clip object behind portal
	lm::mat4 p = lm::mat4::perspectiveProjection(90, float(width) / float(height), dist, 500);
	this->cam.getVP() = p * v;

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glStencilMask(0x00);
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	this->drawGame();
	if (this->debugDrawShape)
		this->physicEngine->getWorld()->debugDrawWorld();

	this->cam.updatePos();
}

void Application::updateGame()
{
	irrklang::vec3df pos(this->cam.getPosition().X(), this->cam.getPosition().Y(), this->cam.getPosition().Z());
	irrklang::vec3df dir(this->cam.getforward().X(), this->cam.getforward().Y(), this->cam.getforward().Z());
	this->soundEngine->getEngine()->setListenerPosition(pos, dir);

	float deltaTime = this->delta.deltaTime * this->delta.deltaScale;
	
	this->physicEngine->update(deltaTime);
	this->scene.update(deltaTime);

	if (this->player != nullptr)
		this->player->updateCam();
}

void Application::updateEditor()
{
	this->ui.updateUI();
}

void Application::updateDeltaTime()
{
	float currentFrame = glfwGetTime();
	this->delta.deltaTime = currentFrame - this->lastFrame;
	this->lastFrame = currentFrame;
}

float Application::getDeltaTime()
{
	return this->delta.deltaTime;
}

unsigned int Application::getWidth()
{
	return this->width;
}

unsigned int Application::getHeight()
{
	return this->height;
}

std::string& Application::getSceneName()
{
	return this->sceneName;
}

Physics::PhysicsEngine& Application::getPhysic()
{
	return *this->physicEngine;
}

Sound::SoundEngine& Application::getSound()
{
	return *this->soundEngine;
}

void Application::setPlayer(Core::Player* player)
{
	this->player = player;
}

Core::Player* Application::getPlayer()
{
	return this->player;
}

Resources::ResourceManager& Application::getResourceManager()
{
	return this->resManager;
}

Core::Scene<Core::GameObject>& Application::getScene()
{
	return this->scene;
}

LowRenderer::Camera& Application::getCam()
{
	return this->cam;
}

UIManager& Application::getUIManager()
{
	return this->ui;
}

void Application::drawGame()
{
	this->scene.draw();

	if (this->player != nullptr)
		this->player->drawPortalsFrame();
}

void Application::drawGameUI()
{
	if (this->player != nullptr)
		this->player->draw();
}

Application::~Application()
{
	delete this->player;

	glfwDestroyWindow(this->window);
	glfwTerminate();
}

void Application::reset()
{
	this->scene.clear();
	this->lights.clear();
	this->player = nullptr;

	this->physicEngine->reset();
	this->sceneName = "NewScene";
}

int main()
{
	Application app(1100, 650, "Portal 3");
	app.run();

	return EXIT_SUCCESS;
}