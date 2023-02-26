#pragma once
#include "ISerializable.h"
#include <LibMaths/Mat4/Mat4.h>
#include "../../Resources/Model.h"
#include "../../Resources/Shader.h"
#include "../../LowRenderer/Camera.h"
#include <GLFW/glfw3.h>
#include <list>
#include <string>
#include "../../Resources/Texture.h"
#include <LibMaths/Vec3/Vec3.h>

namespace Core
{
	class GameObject : public ISerializable
	{
	public:
		static int uniqueId;

		virtual ~GameObject();
		virtual void draw();

		void addChild(GameObject& node);
		void removeChild(GameObject& node);
		void setParent(GameObject& parent);
		void setParent(GameObject* parent);
		GameObject* getParent();
		Resources::Texture* getTexture();
		void setTexture(Resources::Texture* texture);
		Resources::Shader* getShader();
		void setShader(Resources::Shader* shader);
		virtual void updateGlobal();
		virtual void setLocal(const lm::mat4& local);
		void updateLocal();
		std::string& getName();
		virtual const std::string& getType();
		lm::mat4& getLocalTransform();
		lm::mat4& getGlobalTransform();
		lm::mat4& getTransform();
		lm::vec3& getPosition();
		lm::vec3 getGlobalPosition();
		lm::vec3& getRotation();
		lm::mat4 getGlobalRotation(const Core::GameObject& obj, lm::mat4& rotation);
		lm::vec3& getScale();
		lm::vec3 getGlobalScale(const Core::GameObject& obj, lm::vec3& scale);
		Resources::Model* getModel();
		void setModel(Resources::Model* model);
		float& getUniformScale();
		virtual void setVectorMatrix(lm::vec3 position, lm::vec3 rotation, lm::vec3 scale);
		std::list<GameObject*>& getChild();
		std::string& getId();
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		virtual void update(float delta);
		virtual GameObject* clone() = 0;
		virtual void drawUI(GLFWwindow* window);
		virtual void drawRessource(GLFWwindow* window);
		virtual void drawTransform(GLFWwindow* window);
		void calculateVectors();
		lm::vec3& getForward();
		lm::vec3& getUp();
		lm::vec3& getRight();
		float& getAlpha();
		bool countainChild(GameObject* child, bool& flag);

	protected:
		GameObject(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform);
		GameObject(GameObject& obj);
		
		std::string id;
		std::string type;

		lm::mat4 local = lm::mat4::identity;
		lm::mat4 global = lm::mat4::identity;

		lm::vec3* v;
		lm::mat4* vp;
		
		std::string name;
		GameObject* parent = nullptr;
		std::list<GameObject*> child;

		Resources::Model* model;
		Resources::Shader* shader;
		Resources::Texture* texture;


		lm::vec3 position = lm::vec3::zero;
		lm::vec3 rotation = lm::vec3::zero;
		lm::vec3 scale = lm::vec3::unitVal;

		lm::vec3 right = lm::vec3::right;
		lm::vec3 forward = lm::vec3::forward;
		lm::vec3 up = lm::vec3::up;
		float scaleUni = 0;
		float alpha = 1;
	};
}