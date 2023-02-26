#pragma once
#include "../../../Core/DataStructure/PhysicsObject.h"
#include "../../../Core\DataStructure\Objects\Button.h"
namespace Core
{
	class Door : public PhysicsObject
	{
	public:
		Door(const std::string& name, Physics::PhysicsEngine& physic , GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		Door(Door& other);
		~Door();
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		GameObject* clone() override;
		void addButton(Core::Button& button);
		void removeButton(Core::Button& button);
		void draw() override;
		void update(float delta) override;
		virtual void drawRessource(GLFWwindow* window);
		virtual void drawTransform(GLFWwindow* window) override;
		const std::string& getType() override;

	private:
		GLFWwindow* window = nullptr;
		std::list<Button*> buttons;
		bool isOpen = false;
	};
}
