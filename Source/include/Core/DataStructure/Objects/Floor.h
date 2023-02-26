#pragma once
#include "../../../Core/DataStructure/PhysicsObject.h"


namespace Core
{
	class Floor : public PhysicsObject
	{
	public:
		Floor(const std::string& name, Physics::PhysicsEngine& physic, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		Floor(Floor& other);
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		GameObject* clone() override;
		const std::string& getType() override;
	};
}