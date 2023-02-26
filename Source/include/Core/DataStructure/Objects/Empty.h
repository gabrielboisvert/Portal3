#pragma once
#include "../../../Core/DataStructure/GameObject.h"

namespace Core
{
	class Empty : public GameObject
	{
	public:
		Empty(const std::string& name, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform);
		Empty(Empty& other);
		GameObject* clone() override;
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		const std::string& getType() override;
	};
}