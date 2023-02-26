#include "../include/UIManager.h"

#include "../include/Application.h"
#include "ImGui/imgui_internal.h"
#include <iostream>
#include <filesystem>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <sstream>
#include <fstream>
#include "../include/SceneLoader.h"
#include "../../Source/include/Core/DataStructure/Objects/Button.h"
#include "../../Source/include/Core/DataStructure/Objects/CompanionCube.h"
#include "../../Source/include/Core/DataStructure/Objects/Empty.h"
#include "../../Source/include/Core/DataStructure/Objects/Door.h"
#include "../../Source/include/Core/DataStructure/Objects/Floor.h"
#include "../../Source/include/Core/DataStructure/Objects/Platform.h"
#include "../../Source/include/Core/DataStructure/Objects/Player.h"
#include "../../Source/include/Core/DataStructure/Objects/Turret.h"
#include "../../Source/include/Core/DataStructure/Objects/Radio.h"

UIManager::UIManager() {}

UIManager::~UIManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UIManager::init(GLFWwindow* window)
{
	this->window = window;

	//Set up imGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::GetIO().ConfigInputTrickleEventQueue = false;

	this->loadScenes();
	this->loadModels();
	this->loadTextures();
	this->loadShaders();
	this->loadPrefabs();
	this->loadMusics();
	
	//Load first scene in application
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	if (this->scenes.begin() != this->scenes.end())
	{
		SceneLoader::deserializeScene(this->scenes.begin()->second, *app);
		app->getSceneName() = this->scenes.begin()->first;
	}
}

void UIManager::loadScenes()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	std::string path = "Scene/";
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(path))
	{
		if (entry.path().extension() != ".json")
			continue;

		std::string str = entry.path().stem().string();
		if (this->scenes.find(str) != this->scenes.end())
			continue;

		this->scenes[str] = entry.path().string();
	}
}

void UIManager::loadModels()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	std::string path = "assets/";
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(path))
	{
		std::string str = entry.path().string();
		std::replace(str.begin(), str.end(), '\\', '/');

		if (entry.path().extension() != ".fbx" && entry.path().extension() != ".gltf" && entry.path().extension() != ".obj")
			continue;

		std::string fileName = entry.path().stem().string() + '\0';
		if (this->models.find(fileName) != this->models.end())
			continue;

		this->models[fileName] = app->getResourceManager().create<Resources::Model>(str);
	}

	this->models["None"] = nullptr;
}

void UIManager::loadTextures()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	std::string path = "assets/";
	for (std::filesystem::directory_entry entry : std::filesystem::recursive_directory_iterator(path))
	{
		std::string str = entry.path().string();
		std::replace(str.begin(), str.end(), '\\', '/');
		
		if (entry.path().extension() != ".jpg" && entry.path().extension() != ".jpeg" && entry.path().extension() != ".png" && entry.path().extension() != ".bmp")
			continue;

		std::string fileName = entry.path().stem().string();
		if (this->textures.find(fileName) != this->textures.end())
			continue;

		this->textures[fileName] = app->getResourceManager().create<Resources::Texture>(str);
	}

	this->textures["None"] = nullptr;
}

void UIManager::loadShaders()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	
	this->shaders["Unlit"] = app->getResourceManager().create<Resources::Shader>("Unlit", "Source/Shaders/Vertex/UnlitVertex.glsl", "Source/Shaders/Fragment/UnlitFragment.glsl");
	this->shaders["Lit"] = app->getResourceManager().create<Resources::Shader>("Lit", "Source/Shaders/Vertex/LitVertex.glsl", "Source/Shaders/Fragment/LitFragment.glsl");

	this->shaders["None"] = nullptr;
}

void UIManager::loadPrefabs()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	std::string path = "Prefabs/";
	for (std::filesystem::directory_entry entry : std::filesystem::recursive_directory_iterator(path))
	{
		std::string str = entry.path().string();
		std::replace(str.begin(), str.end(), '\\', '/');

		if (entry.path().extension() != ".json")
			continue;

		std::string fileName = entry.path().stem().string();
		if (this->prefabs.find(fileName) != this->prefabs.end())
			continue;

		this->prefabs[fileName] = SceneLoader::deserializeFile(entry.path().string(), fileName, *app);

		Core::PhysicsObject* newObj = dynamic_cast<Core::PhysicsObject*>(this->prefabs[fileName]);
		if (dynamic_cast<Core::PhysicsObject*>(newObj) != nullptr)
			app->getPhysic().getWorld()->removeRigidBody(newObj->getRigidBody()->getRigidBody());
	}
}

void UIManager::loadMusics()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	std::string path = "Sounds/";
	for (std::filesystem::directory_entry entry : std::filesystem::recursive_directory_iterator(path))
	{
		std::string str = entry.path().string();
		std::replace(str.begin(), str.end(), '\\', '/');

		if (entry.path().extension() != ".mp3")
			continue;

		std::string fileName = entry.path().stem().string();

		if (this->musics.find(fileName) != this->musics.end())
			continue;

		this->musics[fileName] = str;
	}
}

void UIManager::createRootNode()
{
	this->shouldCreateNewWindow = false;
	Core::GameObject* newObj = this->createNewObj();

	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	app->getScene().addNode(newObj);

	this->selectedGameObject = newObj;
	this->selectedModel = nullptr;
	this->selectedShader = nullptr;
	this->selectedTexture = nullptr;
	this->selectedPrefab = nullptr;
	this->selectedMusic = nullptr;
}

void UIManager::createChildNode()
{
	this->shouldCreateNewWindow = false;

	Core::GameObject* newObj = this->createNewObj();
	this->parentGameObject->addChild(*newObj);

	this->selectedGameObject = newObj;
	this->selectedModel = nullptr;
	this->selectedShader = nullptr;
	this->selectedTexture = nullptr;
	this->selectedPrefab = nullptr;
	this->selectedMusic = nullptr;
}

Core::GameObject* UIManager::createNewObj()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	Resources::Shader* s = this->shaders["Unlit"];
	Core::GameObject* newObj = nullptr;
	switch (this->currentClassItem.second)
	{
	case Core::E_EMPTY: newObj = new Core::Empty(name, app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity);
		break;
	case Core::E_BUTTON: newObj = new Core::Button(name, app->getPhysic(), *app->GetWindow(), app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity);
		break;
	case Core::E_COMPANION_CUBE: newObj = new Core::CompanionCube(name, app->getPhysic(), *app->GetWindow(),  app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity);
		break;
	case Core::E_DOOR: newObj = new Core::Door(name, app->getPhysic(), *app->GetWindow(), app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity);
		break;
	case Core::E_FLOOR: newObj = new Core::Floor(name, app->getPhysic(), app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity);
		break;
	case Core::E_PLATFORM: newObj = new Core::Platform(name, app->getPhysic(), *app->GetWindow(), app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity, lm::vec3::zero, 0);
		break;
	case Core::E_PLAYER: newObj = new Core::Player(name, app->getPhysic(), *app->GetWindow(), app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity);
									if (app->getPlayer() == nullptr)
									{
										app->setPlayer((Core::Player*)newObj);
										app->getScene().addNode(newObj);
									}
									break;
	case Core::E_TURRET: newObj = new Core::Turret(name, app->getPhysic(), *app->GetWindow(), app->getCam(), this->selectedModel, s, nullptr, 0, 0, lm::mat4::identity);
		break;
	case Core::E_RADIO: newObj = new Core::Radio(name, app->getCam(), this->selectedModel, s, nullptr, lm::mat4::identity, *app->GetWindow(), std::vector<std::string>());
		break;
	case Core::E_DIRECTIONAL_LIGHT: {
										LowRenderer::DirectionalLight* light = new LowRenderer::DirectionalLight();
										newObj = new Core::DirectionalLight(name, app->getCam(), this->selectedModel, this->shaders["Lit"], nullptr, lm::mat4::identity, *light);
										app->addLight(*newObj);
									}
									break;

	case Core::E_POINT_LIGHT: {
								LowRenderer::PointLight* light = new LowRenderer::PointLight();
								newObj = new Core::PointLight(name, app->getCam(), this->selectedModel, this->shaders["Lit"], nullptr, lm::mat4::identity, *light);
								app->addLight(*newObj);
							}
							break;
	case Core::E_SPOT_LIGHT: {
								LowRenderer::SpotLight* light = new LowRenderer::SpotLight();
								newObj = new Core::SpotLight(name, app->getCam(), this->selectedModel, this->shaders["Lit"], nullptr, lm::mat4::identity, *light);
								app->addLight(*newObj);
							}
							break;
	}
	return newObj;
}

void UIManager::updateUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	if (ImGui::IsMouseDown(1))
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		app->getCam().setFocus(true);
		glfwSetCursorPos(this->window, app->getCam().getLastX(), app->getCam().getLastY());

		app->getCam().getPosition() += (app->getCam().calculateFront() * ImGui::GetIO().MouseWheel * this->scrollSpeed);
		app->getCam().updateCameraVectors();
	}
	else
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		app->getCam().setFocus(false);
	}


	this->drawMenuBar();
	this->drawRessourcesUI();
	this->drawScenehierarchy();
	
	if (this->selectedGameObject != nullptr)
		this->selectedGameObject->drawUI(this->window);
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::drawMenuBar()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth(), 25));

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save As", ""))
		{
			this->shoudlSaveAs = true;
			for (unsigned int i = 0; i < app->getSceneName().size(); i++)
				this->name[i] = app->getSceneName()[i];

			for (unsigned int i = app->getSceneName().size(); i < 32; i++)
				this->name[i] = '\0';
		}

		if (ImGui::MenuItem("Save", "ctr + s"))
			this->save();

		if (ImGui::MenuItem("New scene", "ctr + n"))
		{
			this->newScene();
			app->setPlayer(new Core::Player("Player", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::createTransformMatrix(lm::vec3::zero, lm::vec3::zero, lm::vec3::unitVal)));
			app->getScene().addNode(app->getPlayer());
		}

		if (ImGui::BeginMenu("Load scene"))
		{
			std::map<std::string, std::string>::iterator it = this->scenes.begin();
			for (; it != this->scenes.end(); it++)
				if (ImGui::MenuItem(it->first.c_str()))
					this->loadScene(it->first);
			ImGui::EndMenu();
		}
		
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Tools"))
	{

		if (ImGui::MenuItem("ReScan project"))
		{
			this->loadScenes();
			this->loadModels();
			this->loadTextures();
			this->loadShaders();
			this->loadPrefabs();
			this->loadMusics();
		}
		ImGui::EndMenu();
	}

	if (this->shoudlSaveAs)
	{
		ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.4f, app->getHeight() * 0.425f));
		ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.2f, app->getHeight() * 0.15));
		if (ImGui::Begin("Rename"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar)
		{
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			if (ImGui::InputText("name", this->name, 32, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (this->name[0] != '\0')
				{
					this->shoudlSaveAs = false;
					app->getSceneName() = this->name;
					this->isSceneNameChange = true;
					this->save();
				}
			}

			if (ImGui::Button("Save As"))
			{
				if (this->name[0] != '\0')
				{
					this->shoudlSaveAs = false;
					app->getSceneName() = this->name;
					this->isSceneNameChange = true;
					this->save();
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				this->shoudlSaveAs = false;
		}
		ImGui::End();
	}

	ImGui::EndMainMenuBar();
}

void UIManager::drawRessourcesUI()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);

	ImGui::SetNextWindowPos(ImVec2(0, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.25f, app->getHeight() * 0.25));

	ImGui::Begin("Resources", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
	ImGui::BeginTabBar("Resources#left_tabs_bar", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);

	if (ImGui::BeginTabItem("Models"))
	{
		std::map<std::string, Resources::Model*, UIManager::MapComparator>::iterator it = this->models.begin();
		for (; it != this->models.end(); it++)
		{
			ImGui::TreeNodeEx(it->first.c_str(),
				ImGuiTreeNodeFlags_FramePadding
				| ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (this->selectedModel == it->second && this->selectedModel != nullptr ? ImGuiTreeNodeFlags_Selected : 0)
				| ImGuiTreeNodeFlags_Leaf, it->first.c_str());
			ImGui::TreePop();


			if (ImGui::IsItemClicked())
				this->selectedModel = it->second;

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("new_object", it->first.c_str(), sizeof(char) * it->first.length());
				ImGui::EndDragDropSource();
			}
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Shaders"))
	{
		std::map<std::string, Resources::Shader*, UIManager::MapComparator>::iterator it = this->shaders.begin();
		for (; it != this->shaders.end(); it++)
		{
			ImGui::TreeNodeEx(it->first.c_str(),
				ImGuiTreeNodeFlags_FramePadding
				| ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (this->selectedShader == it->second && this->selectedShader != nullptr ? ImGuiTreeNodeFlags_Selected : 0)
				| ImGuiTreeNodeFlags_Leaf, it->first.c_str());
			ImGui::TreePop();


			if (ImGui::IsItemClicked())
				this->selectedShader = it->second;

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("shader", it->first.c_str(), sizeof(char) * it->first.length());
				ImGui::EndDragDropSource();
			}
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Textures"))
	{
		std::map<std::string, Resources::Texture*, UIManager::MapComparator>::iterator it = this->textures.begin();
		for (; it != this->textures.end(); it++)
		{
			ImGui::TreeNodeEx(it->first.c_str(),
				ImGuiTreeNodeFlags_FramePadding
				| ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (this->selectedTexture == it->second && this->selectedTexture != nullptr ? ImGuiTreeNodeFlags_Selected : 0)
				| ImGuiTreeNodeFlags_Leaf, it->first.c_str());
			ImGui::TreePop();


			if (ImGui::IsItemClicked())
				this->selectedTexture = it->second;

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("texture", it->first.c_str(), sizeof(char) * it->first.length());
				ImGui::EndDragDropSource();
			}
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Musics"))
	{
		std::map<std::string, std::string, UIManager::MapComparator>::iterator it = this->musics.begin();
		for (; it != this->musics.end(); it++)
		{
			ImGui::TreeNodeEx(it->first.c_str(),
				ImGuiTreeNodeFlags_FramePadding
				| ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (this->selectedMusic == &it->second ? ImGuiTreeNodeFlags_Selected : 0)
				| ImGuiTreeNodeFlags_Leaf, it->first.c_str());
			ImGui::TreePop();


			if (ImGui::IsItemClicked())
				this->selectedMusic = &it->second;

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("music", it->first.c_str(), sizeof(char)* it->first.length());
				ImGui::EndDragDropSource();
			}
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Prefabs"))
	{
		std::map<std::string, Core::GameObject*, UIManager::MapComparator>::iterator it = this->prefabs.begin();
		for (; it != this->prefabs.end(); it++)
		{
			ImGui::TreeNodeEx(it->first.c_str(),
				ImGuiTreeNodeFlags_FramePadding
				| ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| (this->selectedPrefab == it->second ? ImGuiTreeNodeFlags_Selected : 0)
				| ImGuiTreeNodeFlags_Leaf, it->first.c_str());
			ImGui::TreePop();


			if (ImGui::IsItemClicked())
				this->selectedPrefab = it->second;

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("prefab", it->first.c_str(), sizeof(char) * it->first.length());
				ImGui::EndDragDropSource();
			}
		}

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
	ImGui::End();
}

void UIManager::drawScenehierarchy()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	std::vector<Core::GameObject*>& gameObjects = app->getScene().getGameObjects();

	ImGui::SetNextWindowPos(ImVec2(0, 19));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.25f, (app->getHeight() - 25) * 0.75));

	ImGui::Begin(("Scene - " + app->getSceneName()).c_str(), (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);

	bool firstFrame = false;
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
	{
		ImRect rect = ImGui::GetCurrentWindow()->TitleBarRect();
		if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max, false))
		{
			this->isRightClickOnScene = true;
			firstFrame = true;
		}
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::GetIO().KeyCtrl)
	{
		this->selectedGameObject = nullptr;
		this->selectedModel = nullptr;
		this->selectedShader = nullptr;
		this->selectedTexture = nullptr;
		this->selectedPrefab = nullptr;
		this->selectedMusic = nullptr;
	}
	
	if (this->isRightClickOnScene)
	{
		ImGui::OpenPopup("Right");

		if (ImGui::BeginPopup("Right"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				this->isRightClickOnScene = false;
				this->shouldRenameScene = true;

				for (unsigned int i = 0; i < app->getSceneName().size(); i++)
					this->name[i] = app->getSceneName()[i];

				for (unsigned int i = app->getSceneName().size(); i < 32; i++)
					this->name[i] = '\0';
			}

			if ( (ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered()) || (ImGui::IsMouseClicked(1) && !firstFrame))
				this->isRightClickOnScene = false;
			
			ImGui::EndPopup();
		}
	}

	if (this->shouldRenameScene)
	{
		ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.4f, app->getHeight() * 0.425f));
		ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.2f, app->getHeight() * 0.15));
		if (ImGui::Begin("Rename"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar)
		{
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			if (ImGui::InputText("name", this->name, 32, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (this->name[0] != '\0')
				{
					this->shouldRenameScene = false;
					app->getSceneName() = this->name;
					this->isSceneNameChange = true;
				}
			}

			if (ImGui::Button("Change"))
			{
				if (this->name[0] != '\0')
				{
					this->shouldRenameScene = false;
					app->getSceneName() = this->name;
					this->isSceneNameChange = true;
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				this->shouldRenameScene = false;
		}
		ImGui::End();
	}

	if (ImGui::BeginDragDropTargetCustom(ImRect(ImVec4(0, 0, app->getWidth() * 0.25f, app->getHeight() * 0.75)), 1)) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("new_object", ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			this->parentGameObject = nullptr;
			this->shouldCreateNewWindow = true;
			this->newGameObjectClassType = Core::E_EMPTY;

			std::string newName = (const char*)payload->Data;
			for (unsigned int i = 0; i < newName.size(); i++)
				this->name[i] = newName[i];

			for (unsigned int i = newName.size(); i < 32; i++)
				this->name[i] = '\0';
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("object", ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			if (this->selectedGameObject != nullptr)
			{
				if (this->selectedGameObject->getParent() != nullptr)
				{
					this->selectedGameObject->getParent()->removeChild(*this->selectedGameObject);
					this->selectedGameObject->setParent(nullptr);
					app->getScene().addNode(this->selectedGameObject);
					this->selectedModel = nullptr;
					this->selectedShader = nullptr;
					this->selectedTexture = nullptr;
					this->selectedPrefab = nullptr;
					this->selectedMusic = nullptr;
					this->draggedGameObject = nullptr;
				}
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("prefab", ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			if (this->selectedPrefab != nullptr)
			{
				app->getScene().addNode(this->selectedPrefab->clone());

				this->selectedModel = nullptr;
				this->selectedShader = nullptr;
				this->selectedTexture = nullptr;
				this->selectedPrefab = nullptr;
				this->selectedMusic = nullptr;
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (this->shouldCreateNewWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.4f, app->getHeight() * 0.425f));
		ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.2f, app->getHeight() * 0.2));

		if (ImGui::Begin("New Object"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar)
		{
			if (ImGui::InputText("name", this->name, 32, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
				if (this->name[0] != '\0')
					if (this->parentGameObject == nullptr)
						this->createRootNode();
					else
						this->createChildNode();
			
			if (ImGui::BeginCombo("Type", this->currentClassItem.first))
			{
				for (int i = 0; i < this->classItems.size(); i++)
				{
					bool is_selected = (this->currentClassItem == this->classItems[i]);
					if (ImGui::Selectable(this->classItems[i].first, is_selected))
						this->currentClassItem = this->classItems[i];
					
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Create"))
				if (this->name[0] != '\0')
					if (this->parentGameObject == nullptr)
						this->createRootNode();
					else
						this->createChildNode();

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				this->shouldCreateNewWindow = false;
		}
		ImGui::End();
	}

	if (this->shouldRenameGameObject)
	{
		ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.4f, app->getHeight() * 0.425f));
		ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.2f, app->getHeight() * 0.15));
		if (ImGui::Begin("Rename"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar)
		{
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
			if (ImGui::InputText("name", this->name, 32, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (this->name[0] != '\0')
				{
					this->shouldRenameGameObject = false;
					this->selectedGameObject->getName() = this->name;
				}
			}

			if (ImGui::Button("Change"))
			{
				if (this->name[0] != '\0')
				{
					this->shouldRenameGameObject = false;
					this->selectedGameObject->getName() = this->name;
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				this->shouldRenameGameObject = false;
		}
		ImGui::End();
	}

	if (ImGui::BeginPopupContextWindow("emptyRightClick"))
	{
		if (ImGui::BeginMenu("New Game Object"))
		{
			if (ImGui::MenuItem("Empty"))
			{
				Core::GameObject* obj = new Core::Empty("GameObject", app->getCam(), nullptr, nullptr, nullptr, lm::mat4::identity);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Button"))
			{
				Core::GameObject* obj = new Core::Button("Button", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::identity);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Companion cube"))
			{
				Core::GameObject* obj = new Core::CompanionCube("Companion cube", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::identity);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Door"))
			{
				Core::GameObject* obj = new Core::Door("Door", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::identity);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Floor"))
			{
				Core::GameObject* obj = new Core::Floor("Floor", app->getPhysic(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::identity);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Platform"))
			{
				Core::GameObject* obj = new Core::Platform("Platform", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::identity, lm::vec3::zero, 0);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Player"))
			{
				Core::GameObject* obj = new Core::Player("Player", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::identity);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Turret"))
			{
				Core::GameObject* obj = new Core::Turret("Turret", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, 0, 0, lm::mat4::identity);
				app->getScene().addNode(obj);
			}
			else if (ImGui::MenuItem("Radio"))
			{
				Core::GameObject* obj = new Core::Radio("Radio", app->getCam(), this->selectedModel, nullptr, nullptr, lm::mat4::identity, *app->GetWindow(), std::vector<std::string>());
				app->getScene().addNode(obj);
			}

			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					LowRenderer::DirectionalLight* light = new LowRenderer::DirectionalLight();
					Core::GameObject* obj = new Core::DirectionalLight("Directional Light", app->getCam(), nullptr, this->shaders["Lit"], nullptr, lm::mat4::identity, *light);
					app->addLight(*obj);
					app->getScene().addNode(obj);
				}
				else if (ImGui::MenuItem("Point Light"))
				{
					LowRenderer::PointLight* light = new LowRenderer::PointLight();
					Core::GameObject* obj = new Core::PointLight("Point Light", app->getCam(), nullptr, this->shaders["Lit"], nullptr, lm::mat4::identity, *light);
					app->addLight(*obj);
					app->getScene().addNode(obj);
				}
				else if (ImGui::MenuItem("Spot Light"))
				{
					LowRenderer::SpotLight* light = new LowRenderer::SpotLight();
					Core::GameObject* obj = new Core::SpotLight("Spot Light", app->getCam(), nullptr, this->shaders["Lit"], nullptr, lm::mat4::identity, *light);
					app->addLight(*obj);
					app->getScene().addNode(obj);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Paste", "ctr + v", false, this->copiedGameObject != nullptr))
			this->paste();

		ImGui::EndPopup();
	}

	for (unsigned int i = 0; i < gameObjects.size(); i++)
		this->drawSceneObject(*gameObjects[i]);

	ImGui::End();
}

void UIManager::drawSceneObject(Core::GameObject& object)
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);

	bool nodeOpen = ImGui::TreeNodeEx(object.getId().c_str(),
		ImGuiTreeNodeFlags_FramePadding
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| (this->selectedGameObject == &object ? ImGuiTreeNodeFlags_Selected : 0)
		| (object.getChild().size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0), (object.getName() + " - " + object.getType()).c_str());
	ImGui::PushID(object.getId().c_str());
	
	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("object", &object.getName(), sizeof(char) * object.getName().length());
		this->draggedGameObject = &object;
		ImGui::EndDragDropSource();
	}
	else if (ImGui::IsItemClicked() && !ImGui::GetIO().KeyCtrl)
	{
		if (ImGui::IsMouseDoubleClicked(0) && object.getModel() != nullptr)
		{
			app->getCam().getPitch() = -object.getRotation().X();
			app->getCam().getYaw() = -90;
			app->getCam().getPosition() = object.getPosition() - (app->getCam().calculateFront() * this->scrollSpeed);

			app->getCam().updateCameraVectors();
		}

		if (!this->shouldRenameGameObject)
		{
			this->selectedGameObject = &object;
			this->selectedModel = nullptr;
			this->selectedShader = nullptr;
			this->selectedTexture = nullptr;
			this->selectedPrefab = nullptr;
			this->selectedMusic = nullptr;
		}
	}

	if (ImGui::BeginPopupContextItem("RightClickContext"))
	{
		this->selectedGameObject = &object;
		this->selectedModel = nullptr;
		this->selectedShader = nullptr;
		this->selectedTexture = nullptr;
		this->selectedPrefab = nullptr;
		this->selectedMusic = nullptr;

		if (ImGui::MenuItem("Create/Save Prefab"))
		{
			lm::vec3 oldPos(this->selectedGameObject->getPosition());
			this->selectedGameObject->getPosition() = lm::vec3();
			this->selectedGameObject->setLocal(lm::mat4::createTransformMatrix(this->selectedGameObject->getPosition(), this->selectedGameObject->getRotation(), this->selectedGameObject->getScale()));


			rapidjson::StringBuffer ss;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(ss);
			this->selectedGameObject->serialize(writer);

			std::ofstream file(("Prefabs/" + this->selectedGameObject->getName() + ".json").c_str());
			std::string s = ss.GetString();
			file << s;
			file.close();


			if (this->prefabs.count(this->selectedGameObject->getName()) != 0)
				delete this->prefabs[this->selectedGameObject->getName()];
			this->prefabs[this->selectedGameObject->getName()] = this->selectedGameObject->clone();

			Core::PhysicsObject* newObj = dynamic_cast<Core::PhysicsObject*>(this->prefabs[this->selectedGameObject->getName()]);
			if (dynamic_cast<Core::PhysicsObject*>(newObj) != nullptr)
				app->getPhysic().getWorld()->removeRigidBody(newObj->getRigidBody()->getRigidBody());

			this->selectedGameObject->getPosition() = oldPos;
			this->selectedGameObject->setLocal(lm::mat4::createTransformMatrix(this->selectedGameObject->getPosition(), this->selectedGameObject->getRotation(), this->selectedGameObject->getScale()));
		}

		if (ImGui::MenuItem("Rename"))
		{
			this->shouldRenameGameObject = true;

			for (unsigned int i = 0; i < this->selectedGameObject->getName().size(); i++)
				this->name[i] = this->selectedGameObject->getName()[i];

			for (unsigned int i = this->selectedGameObject->getName().size(); i < 32; i++)
				this->name[i] = '\0';
		}

		if (ImGui::MenuItem("Copy", "ctr + c"))
			this->copy();


		if (ImGui::MenuItem("Paste", "ctr + v", false, this->copiedGameObject != nullptr))
		{
			Core::GameObject* obj = this->copiedGameObject->clone();
			object.addChild(*obj);
		}

		if (ImGui::MenuItem("Delete", "del"))
		{
			this->selectedGameObject = &object;
			this->selectedModel = nullptr;
			this->selectedShader = nullptr;
			this->selectedTexture = nullptr;
			this->selectedPrefab = nullptr;
			this->selectedMusic = nullptr;

			this->deleteGameObject();
			ImGui::EndPopup();
			ImGui::TreePop();
			ImGui::PopID();
			return;
		}
		ImGui::EndPopup();
	}

	

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("new_object"))
		{
			if (this->selectedModel != nullptr)
			{
				this->parentGameObject = &object;
				this->shouldCreateNewWindow = true;
				this->newGameObjectClassType = Core::E_EMPTY;

				std::string newName = (const char*)payload->Data;
				for (unsigned int i = 0; i < newName.size(); i++)
					this->name[i] = newName[i];

				for (unsigned int i = newName.size(); i < 32; i++)
					this->name[i] = '\0';
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("object"))
		{
			bool flag = false;
			this->selectedGameObject->countainChild(&object, flag);
			if (&object != this->selectedGameObject && !flag)
			{
				if (this->selectedGameObject->getParent() == nullptr)
					app->getScene().removeNode(*this->selectedGameObject);
				else
					this->selectedGameObject->getParent()->removeChild(*this->selectedGameObject);

				object.addChild(*this->selectedGameObject);
				this->selectedGameObject = nullptr;
				this->draggedGameObject = nullptr;
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("prefab"))
		{
			if (this->selectedPrefab != nullptr)
			{
				object.addChild(*this->selectedPrefab->clone());

				this->selectedModel = nullptr;
				this->selectedShader = nullptr;
				this->selectedTexture = nullptr;
				this->selectedPrefab = nullptr;
				this->selectedMusic = nullptr;
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (nodeOpen)
	{
		for (std::list<Core::GameObject*>::iterator it = object.getChild().begin(); it != object.getChild().end(); it++)
			this->drawSceneObject(**it);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void UIManager::deleteGameObject()
{
	if (this->selectedGameObject == nullptr)
		return;

	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	app->removeLight(*this->selectedGameObject);
	
	if (this->selectedGameObject->getParent() == nullptr)
	{
		app->getScene().removeNode(*this->selectedGameObject);
		delete this->selectedGameObject;
		this->selectedGameObject = nullptr;
	}
	else
	{
		this->selectedGameObject->getParent()->removeChild(*this->selectedGameObject);
		delete this->selectedGameObject;
		this->selectedGameObject = nullptr;
	}
}

bool UIManager::isRenameWindowActif()
{
	return this->shouldRenameGameObject || this->shouldRenameScene || this->shoudlSaveAs;
}

Resources::Model* UIManager::getSelectedModel()
{
	return this->selectedModel;
}

Resources::Shader* UIManager::getSelectedShader()
{
	return this->selectedShader;
}

std::map<std::string, Resources::Shader*, UIManager::MapComparator>& UIManager::getShaders()
{
	return this->shaders;
}

Resources::Texture* UIManager::getSelectedTexture()
{
	return this->selectedTexture;
}

Core::GameObject* UIManager::getDraggedGameObject()
{
	return this->draggedGameObject;
}

Core::GameObject* UIManager::getSelectedGameObject()
{
	return this->selectedGameObject;
}

void UIManager::setSelectedGameObject(Core::GameObject* obj)
{
	this->selectedGameObject = obj;
}

std::string* UIManager::getDraggedMusic()
{
	return this->selectedMusic;
}

void UIManager::copy()
{
	if (this->selectedGameObject == nullptr)
		return;

	this->copiedGameObject = this->selectedGameObject;
}

void UIManager::paste()
{
	if (this->copiedGameObject == nullptr)
		return;

	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	Core::GameObject* obj = this->copiedGameObject->clone();
	app->getScene().addNode(obj);
}

void UIManager::loadScene(const std::string sceneName)
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);

	this->newScene();
	SceneLoader::deserializeScene(this->scenes[sceneName], *app);
	app->getSceneName() = sceneName;

	if (app->getPlayer() == nullptr)
		app->setPlayer(new Core::Player("Player", app->getPhysic(), *app->GetWindow(), app->getCam(), nullptr, nullptr, nullptr, lm::mat4::createTransformMatrix(lm::vec3::zero, lm::vec3::zero, lm::vec3::unitVal)));
}

void UIManager::save()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	SceneLoader::serializeScene("Scene/" + app->getSceneName() + ".json", app->getScene());

	if (this->isSceneNameChange)
		this->loadScenes();
}

void UIManager::newScene()
{
	Application* app = (Application*)glfwGetWindowUserPointer(this->window);
	app->reset();
	this->shaders["Lit"]->clearLights();

	this->selectedGameObject = nullptr;
	this->selectedModel = nullptr;
	this->selectedShader = nullptr;
	this->selectedTexture = nullptr;
	this->selectedPrefab = nullptr;
	this->selectedMusic = nullptr;
}

bool UIManager::MapComparator::operator()(const std::string& a, const std::string& b) const
{
	if (a == "None")
		return false;
	else if (b == "None")
		return true;

	return a < b;
}