#include "../include/SceneLoader.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <sstream>
#include <fstream>
#include "../include/core/DataStructure/ISerializable.h"
#include "../../Source/include/Core/DataStructure/Objects/CompanionCube.h"
#include "../../Source/include/Core/DataStructure/Objects/Empty.h"
#include "../../Source/include/Core/DataStructure/Objects/Turret.h"

#include "../../Source/include/Core/DataStructure/Objects/Player.h"
#include "../../Source/include/Core/DataStructure/Objects/Platform.h"
#include "../../Source/include/Core/DataStructure/Objects/Floor.h"

#include "../../Source/include/Core/DataStructure/Objects/Radio.h"

std::map<std::string, Core::Button*> SceneLoader::tmpButtonToLink;
std::map<Core::Door*, std::list<std::string>> SceneLoader::tmpDoor;

void SceneLoader::serializeScene(const std::string& filePath, Core::Scene<Core::GameObject>& scene)
{
	rapidjson::StringBuffer ss;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(ss);

	writer.StartObject();

	writer.Key("GameObjects");
	writer.StartArray();

	std::vector<Core::GameObject*>& gameObjects = scene.getGameObjects();
	for (unsigned int i = 0; i < gameObjects.size(); i++)
		gameObjects[i]->serialize(writer);

	writer.EndArray();
	writer.EndObject();

	std::ofstream file(filePath);
	std::string s = ss.GetString();
	file << s;
	file.close();
}

void SceneLoader::deserializeScene(const std::string& filePath, Application& app)
{
	std::ifstream file(filePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	rapidjson::Document doc;
	bool result = doc.Parse(buffer.str().c_str()).HasParseError();

	const rapidjson::Value& objects = doc["GameObjects"].GetArray();
	for (unsigned int i = 0; i < objects.Size(); i++)
		app.getScene().addNode(deserializeObj(objects[i], app));

	//link door to button
	std::map<Core::Door*, std::list<std::string>>::iterator door = SceneLoader::tmpDoor.begin();
	for (; door != SceneLoader::tmpDoor.end(); door++)
		for (std::list<std::string>::iterator it = door->second.begin(); it != door->second.end(); it++)
			door->first->addButton(*SceneLoader::tmpButtonToLink[*it]);

	SceneLoader::tmpButtonToLink.clear();
	SceneLoader::tmpDoor.clear();
}

Core::GameObject* SceneLoader::deserializeFile(const std::string& filePath, const std::string& objecName, Application& app)
{
	std::ifstream file(filePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	rapidjson::Document doc;
	bool result = doc.Parse(buffer.str().c_str()).HasParseError();

	Core::GameObject* newObj = SceneLoader::deserializeObj(doc.GetObject(), app);
	newObj->getName() = objecName;

	return newObj;
}

Core::GameObject* SceneLoader::deserializeObj(const rapidjson::Value& object, Application& app)
{
	std::string id = object["Id"].GetString();

	std::string name = object["Name"].GetString();
	Resources::Model* m = (object["Model"].GetString()[0] != '\0' ? app.getResourceManager().create<Resources::Model>(object["Model"].GetString()) : nullptr);
	
	Resources::Shader* s = nullptr;
	if (object["Shader"].GetStringLength() != 0)
		s = app.getUIManager().getShaders()[object["Shader"].GetString()];
	
	Resources::Texture* t = (object["Texture"].GetString()[0] != '\0' ? app.getResourceManager().create<Resources::Texture>(object["Texture"].GetString()) : nullptr);

	const rapidjson::Value& matrix = object["Transform"].GetObj();
	lm::vec3 positon(matrix["Position"][0].GetDouble(), matrix["Position"][1].GetDouble(), matrix["Position"][2].GetDouble());
	lm::vec3 rotation(matrix["Rotation"][0].GetDouble(), matrix["Rotation"][1].GetDouble(), matrix["Rotation"][2].GetDouble());
	lm::vec3 scale(matrix["Scale"][0].GetDouble(), matrix["Scale"][1].GetDouble(), matrix["Scale"][2].GetDouble());
	
	lm::vec3 rigidBodySize;
	if (object.HasMember("RigidBodySize"))
		rigidBodySize = lm::vec3(object["RigidBodySize"][0].GetDouble(), object["RigidBodySize"][1].GetDouble(), object["RigidBodySize"][2].GetDouble());

	Core::GameObject* newObj = nullptr;
	if (object["ClassType"].GetInt() == Core::E_COMPANION_CUBE)
		newObj = new Core::CompanionCube(name, app.getPhysic(), *app.GetWindow(), app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), rigidBodySize);
	else if (object["ClassType"].GetInt() == Core::E_EMPTY)
		newObj = new Core::Empty(name, app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale));
	else if (object["ClassType"].GetInt() == Core::E_TURRET)
	{
		newObj = new Core::Turret(name, app.getPhysic(), *app.GetWindow(), app.getCam(), m, s, t, object["Radius"].GetDouble(), object["ViewAngle"].GetDouble(), lm::mat4::createTransformMatrix(positon, rotation, scale), rigidBodySize);
	}
	else if (object["ClassType"].GetInt() == Core::E_BUTTON)
	{
		newObj = new Core::Button(name, app.getPhysic(), *app.GetWindow(), app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), rigidBodySize);

		SceneLoader::tmpButtonToLink[id] = (Core::Button*)newObj;
	}
	else if (object["ClassType"].GetInt() == Core::E_DOOR)
	{
		newObj = new Core::Door(name, app.getPhysic(), *app.GetWindow(), app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), rigidBodySize);
		
		const rapidjson::Value& buttons = object["Buttons"].GetArray();
		for (unsigned int i = 0; i < buttons.Size(); i++)
			SceneLoader::tmpDoor[(Core::Door*)newObj].push_back(buttons[i].GetString());
	}
	else if (object["ClassType"].GetInt() == Core::E_PLAYER)
	{
		newObj = new Core::Player(name, app.getPhysic(), *app.GetWindow(), app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), object["Grab range"].GetDouble(), object["Fire range"].GetDouble(), object["Jump force"].GetDouble(), rigidBodySize);
		app.setPlayer((Core::Player*)newObj);
	}
	else if (object["ClassType"].GetInt() == Core::E_PLATFORM)
	{
		lm::vec3 max(object["Max"][0].GetDouble(), object["Max"][1].GetDouble(), object["Max"][2].GetDouble());
		newObj = new Core::Platform(name, app.getPhysic(), *app.GetWindow(), app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), max, object["Duration"].GetDouble(), rigidBodySize);
	}
	else if (object["ClassType"].GetInt() == Core::E_FLOOR)
		newObj = new Core::Floor(name, app.getPhysic(), app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), rigidBodySize);
	else if (object["ClassType"].GetInt() == Core::E_RADIO)
	{
		const rapidjson::Value& musics = object["Musics"].GetArray();
		std::vector<std::string> musicVec;
		for (unsigned int i = 0; i < musics.Size(); i++)
			musicVec.emplace_back(musics[i].GetString());
		newObj = new Core::Radio(name, app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), *app.GetWindow(), musicVec);
	}
	else if (object["ClassType"].GetInt() == Core::E_DIRECTIONAL_LIGHT)
	{
		lm::vec4 diffuse(object["Diffuse"][0].GetDouble(), object["Diffuse"][1].GetDouble(), object["Diffuse"][2].GetDouble(), object["Diffuse"][3].GetDouble());
		lm::vec4 ambient(object["Ambient"][0].GetDouble(), object["Ambient"][1].GetDouble(), object["Ambient"][2].GetDouble(), object["Ambient"][3].GetDouble());
		lm::vec4 specular(object["Specular"][0].GetDouble(), object["Specular"][1].GetDouble(), object["Specular"][2].GetDouble(), object["Specular"][3].GetDouble());

		LowRenderer::DirectionalLight* light = new LowRenderer::DirectionalLight(positon, diffuse, ambient, specular);
		newObj = new Core::DirectionalLight(name, app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), *light);
		
		app.addLight(*newObj);
	}
	else if (object["ClassType"].GetInt() == Core::E_POINT_LIGHT)
	{
		lm::vec4 diffuse(object["Diffuse"][0].GetDouble(), object["Diffuse"][1].GetDouble(), object["Diffuse"][2].GetDouble(), object["Diffuse"][3].GetDouble());
		lm::vec4 ambient(object["Ambient"][0].GetDouble(), object["Ambient"][1].GetDouble(), object["Ambient"][2].GetDouble(), object["Ambient"][3].GetDouble());
		lm::vec4 specular(object["Specular"][0].GetDouble(), object["Specular"][1].GetDouble(), object["Specular"][2].GetDouble(), object["Specular"][3].GetDouble());

		LowRenderer::PointLight* light = new LowRenderer::PointLight(positon, object["Constant"].GetDouble(), object["Linear"].GetDouble(), object["Quadratic"].GetDouble(), diffuse, ambient, specular);
		newObj = new Core::PointLight(name, app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), *light);

		app.addLight(*newObj);
	}
	else if (object["ClassType"].GetInt() == Core::E_SPOT_LIGHT)
	{
		lm::vec4 diffuse(object["Diffuse"][0].GetDouble(), object["Diffuse"][1].GetDouble(), object["Diffuse"][2].GetDouble(), object["Diffuse"][3].GetDouble());
		lm::vec4 ambient(object["Ambient"][0].GetDouble(), object["Ambient"][1].GetDouble(), object["Ambient"][2].GetDouble(), object["Ambient"][3].GetDouble());
		lm::vec4 specular(object["Specular"][0].GetDouble(), object["Specular"][1].GetDouble(), object["Specular"][2].GetDouble(), object["Specular"][3].GetDouble());

		lm::vec3 direction(object["Direction"][0].GetDouble(), object["Direction"][1].GetDouble(), object["Direction"][2].GetDouble());

		LowRenderer::SpotLight* light = new LowRenderer::SpotLight(positon, direction, object["CutOff"].GetDouble(), object["OuterCutOff"].GetDouble(), object["Constant"].GetDouble(), object["Linear"].GetDouble(), object["Quadratic"].GetDouble(), diffuse, ambient, specular);
		newObj = new Core::SpotLight(name, app.getCam(), m, s, t, lm::mat4::createTransformMatrix(positon, rotation, scale), *light);

		app.addLight(*newObj);
	}

	const rapidjson::Value& childs = object["Childs"].GetArray();
	for (unsigned int i = 0; i < childs.Size(); i++)
		newObj->addChild(*deserializeObj(childs[i], app));

	newObj->getId() = id;
	newObj->setVectorMatrix(positon, rotation, scale);
	newObj->getAlpha() = object["Alpha"].GetDouble();
	Core::GameObject::uniqueId = std::stoi(id) + 1;
	return newObj;
}

