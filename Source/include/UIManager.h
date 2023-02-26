#pragma once

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

#include "Resources/Shader.h"
#include "Resources/Model.h"
#include "Core/DataStructure/GameObject.h"
#include <map>
#include <string>

class UIManager
{
private:
	struct MapComparator {
		bool operator()(const std::string& a, const std::string& b) const;
	};

	GLFWwindow* window = nullptr;
	char name[32] = "";
	bool shouldRenameGameObject = false;

	bool isRightClickOnScene = false;
	bool shouldRenameScene = false;

	bool isSceneNameChange = false;

	bool shoudlSaveAs = false;

	bool shouldCreateNewWindow = false;

	float scrollSpeed = 10;

	std::map<std::string, std::string> scenes;
	std::map<std::string, Resources::Model*, MapComparator> models;
	std::map<std::string, Resources::Texture*, MapComparator> textures;
	std::map<std::string, Resources::Shader*, MapComparator> shaders;
	std::map<std::string, Core::GameObject*, MapComparator> prefabs;
	std::map<std::string, std::string, MapComparator> musics;

	Resources::Model* selectedModel = nullptr;
	Resources::Shader* selectedShader = nullptr;
	Resources::Texture* selectedTexture = nullptr;
	Core::GameObject* selectedGameObject = nullptr;
	Core::GameObject* draggedGameObject = nullptr;
	Core::GameObject* selectedPrefab = nullptr;
	std::string* selectedMusic = nullptr;

	Core::GameObject* copiedGameObject = nullptr;
	Core::GameObject* parentGameObject = nullptr;

	int newGameObjectClassType = Core::E_EMPTY;

	std::vector<std::pair<const char*, int>> classItems = { std::pair("Empty", Core::E_EMPTY), 
															std::pair("Button", Core::E_BUTTON), 
															std::pair("Companion cube", Core::E_COMPANION_CUBE), 
															std::pair("Door", Core::E_DOOR), 
															std::pair("Floor", Core::E_FLOOR), 
															std::pair("Platform", Core::E_PLATFORM), 
															std::pair("Player", Core::E_PLAYER), 
															std::pair("Turret", Core::E_TURRET), 
															std::pair("Directional Light", Core::E_DIRECTIONAL_LIGHT), 
															std::pair("Point Light", Core::E_POINT_LIGHT), 
															std::pair("Spot Light", Core::E_SPOT_LIGHT) 
														};
	std::pair<const char*, int> currentClassItem = classItems[0];

	void loadScenes();
	void loadModels();
	void loadTextures();
	void loadShaders();
	void loadPrefabs();
	void loadMusics();

	void createRootNode();
	void createChildNode();
	Core::GameObject* createNewObj();

public:
	UIManager();
	~UIManager();
	void init(GLFWwindow* window);

	void updateUI();

	void drawMenuBar();
	void drawRessourcesUI();
	void drawScenehierarchy();
	void drawSceneObject(Core::GameObject& object);

	void newScene();
	void save();
	void deleteGameObject();
	void copy();
	void paste();

	void loadScene(const std::string sceneName);

	bool isRenameWindowActif();

	Resources::Model* getSelectedModel();
	Resources::Shader* getSelectedShader();
	Resources::Texture* getSelectedTexture();
	Core::GameObject* getDraggedGameObject();
	Core::GameObject* getSelectedGameObject();
	void setSelectedGameObject(Core::GameObject* obj);
	std::string* getDraggedMusic();
	std::map<std::string, Resources::Shader*, MapComparator>& getShaders();
};