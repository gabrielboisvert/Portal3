#include "..\..\..\..\include\Core\DataStructure\Objects\Radio.h"
#include "../../../../include/Application.h"

Core::Radio::Radio(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, GLFWwindow& window, const std::vector<std::string>& musics) : GameObject(name, cam, model, shader, texture, localTransform), window(&window)
{
	this->type = "(Radio)";

	Application* app = (Application*)glfwGetWindowUserPointer(&window);

	this->music = new Sound::Music(this->position, app->getSound());

	for (unsigned int i = 0; i < musics.size(); i++)
		this->music->addMusic(musics[i]);
}

Core::Radio::Radio(Radio& other) : Core::GameObject(other)
{
	this->window = other.window;

	Application* app = (Application*)glfwGetWindowUserPointer(this->window);

	this->music = new Sound::Music(this->position, app->getSound());
	for (unsigned int i = 0; i < other.music->getPlaylist().size(); i++)
		this->music->addMusic(other.music->getPlaylist()[i]);
	this->music->playMusic();
}

Core::Radio::~Radio()
{
	delete this->music;
}

Core::GameObject* Core::Radio::clone()
{
	return new Core::Radio(*this);
}

void Core::Radio::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.Key("ClassType");
	writer.Int(E_RADIO);
	GameObject::serialize(writer);
	
	writer.Key("Musics");
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartArray();
	for (unsigned int i = 0; i < this->music->getPlaylist().size(); i++)
		writer.String(this->music->getPlaylist()[i].c_str());
	writer.EndArray();
	writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
	
	writer.EndObject();
}

void Core::Radio::update(float deltaTime)
{
	this->music->update();
}

void Core::Radio::drawRessource(GLFWwindow* window)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	ImGui::SetNextWindowPos(ImVec2(app->getWidth() * 0.25f, app->getHeight() * 0.75));
	ImGui::SetNextWindowSize(ImVec2(app->getWidth() * 0.25f, app->getHeight() * 0.25));

	ImGui::Begin("Object Resource", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing);

	bool nodeOpen = ImGui::TreeNodeEx("ID_Model", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, ("Model -> " + (this->model != nullptr ? this->model->getName() : "None")).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("new_object"))
			this->model = app->getUIManager().getSelectedModel();

		ImGui::EndDragDropTarget();
	}
	if (nodeOpen)
		ImGui::TreePop();


	nodeOpen = ImGui::TreeNodeEx("ID_shader", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, ("Shader -> " + (this->shader != nullptr ? this->shader->getName() : "None")).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("shader"))
			this->shader = app->getUIManager().getSelectedShader();

		ImGui::EndDragDropTarget();
	}
	if (nodeOpen)
		ImGui::TreePop();

	nodeOpen = ImGui::TreeNodeEx("ID_texture", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, ("Texture -> " + (this->texture != nullptr ? this->texture->getName() : "None")).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("texture"))
			this->texture = app->getUIManager().getSelectedTexture();

		ImGui::EndDragDropTarget();
	}
	if (nodeOpen)
		ImGui::TreePop();

	nodeOpen = ImGui::TreeNodeEx("Musics", ImGuiTreeNodeFlags_FramePadding | (this->music->getPlaylist().size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0), "Musics");
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("music"))
			this->music->addMusic(*app->getUIManager().getDraggedMusic());

		ImGui::EndDragDropTarget();
	}

	if (nodeOpen)
	{
		for (std::vector<std::string>::iterator it = this->music->getPlaylist().begin(); it != this->music->getPlaylist().end(); it++)
		{
			ImGui::TreeNodeEx((*it).c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf, (*it).c_str());
			if (ImGui::BeginPopupContextItem("RightClickContext"))
			{
				if (ImGui::MenuItem("remove"))
				{
					this->music->removeMusic(it);
					ImGui::EndPopup();
					ImGui::TreePop();
					break;
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}

	if (nodeOpen)
		ImGui::TreePop();

	ImGui::End();
}