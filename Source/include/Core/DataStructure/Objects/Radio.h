#include "../GameObject.h"
#include "../../../Sound/Music.h"

namespace Core
{
	class Radio : public GameObject
	{
	private:
		GLFWwindow* window = nullptr;
		Sound::Music* music = nullptr;
	
	public:
		Radio(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, GLFWwindow& window, const std::vector<std::string>& musics);
		Radio(Radio& other);
		~Radio();
		virtual GameObject* clone() override;
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		void update(float deltaTime) override;
		void drawRessource(GLFWwindow* window) override;
	};
}