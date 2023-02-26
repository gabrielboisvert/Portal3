#pragma once
#include "../GameObject.h"
#include "../../../LowRenderer/Light.h"

namespace Core
{
	class DirectionalLight : public GameObject
	{
	private:
		LowRenderer::DirectionalLight* light;

	public:
		DirectionalLight(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, LowRenderer::DirectionalLight& light);
		DirectionalLight(DirectionalLight& other);
		~DirectionalLight();

		void update(float delta) override;
		void drawTransform(GLFWwindow* window) override;
		GameObject* clone() override;
		std::string& getType() override;
		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		void setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale) override;
	};

	class PointLight : public GameObject
	{
	private:
		LowRenderer::PointLight* light;

	public:
		PointLight(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, LowRenderer::PointLight& light);
		PointLight(PointLight& other);
		~PointLight();

		void update(float delta) override;
		void drawTransform(GLFWwindow* window) override;
		GameObject* clone() override;
		std::string& getType() override;
		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		void setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale) override;
	};

	class SpotLight : public GameObject
	{
	private:
		LowRenderer::SpotLight* light;

	public:
		SpotLight(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, LowRenderer::SpotLight& light);
		SpotLight(SpotLight& other);
		~SpotLight();

		void update(float delta) override;
		void drawTransform(GLFWwindow* window) override;
		GameObject* clone() override;
		std::string& getType() override;
		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		void setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale) override;
	};
}