#pragma once

#include "Core/DataStructure/Scene.h"
#include "Core/DataStructure/GameObject.h"
#include "Core/DataStructure/Objects/Button.h"
#include "Core/DataStructure/Objects/Door.h"

#include "Application.h"
#include <rapidjson/document.h>

class SceneLoader
{
private:
	static std::map<std::string, Core::Button*> tmpButtonToLink;
	static std::map<Core::Door*, std::list<std::string>> tmpDoor;

public:
	static void SceneLoader::serializeScene(const std::string& filePath, Core::Scene<Core::GameObject>& scene);
	static void SceneLoader::deserializeScene(const std::string& filePath, Application& app);
	static Core::GameObject* SceneLoader::deserializeFile(const std::string& filePath, const std::string& objecName, Application& app);
	static Core::GameObject* SceneLoader::deserializeObj(const rapidjson::Value& object, Application& app);

};