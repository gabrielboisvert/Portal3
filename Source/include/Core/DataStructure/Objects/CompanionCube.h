#pragma once
#include "../../Source/include/Core/DataStructure/InteractibleObject.h"
#include "Portal.h"

namespace Core
{
	class CompanionCube : public InteractibleObject
	{
	public:
		CompanionCube(const std::string& name, Physics::PhysicsEngine& physic, GLFWwindow& window, LowRenderer::Camera& cam, Resources::Model* model, Resources::Shader* shader, Resources::Texture* texture, const lm::mat4& localTransform, const lm::vec3& rigidBodySize = lm::vec3::unitVal);
		CompanionCube(CompanionCube& other);
		void virtual serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
		GameObject* clone() override;
		const std::string& getType() override;
	};
}
