#pragma once
#include <LibMaths/Vec4/Vec4.h>

namespace LowRenderer
{
	class Light
	{
		protected:
			lm::vec4 diffuse;
			lm::vec4 ambient;
			lm::vec4 specular;

		public:
			Light(const lm::vec4& diffuse = lm::vec4(1, 1, 1, 0.8f), const lm::vec4& ambient = lm::vec4(1, 1, 1, 0.3f), const lm::vec4& specular = lm::vec4(1, 1, 1, 1));

			lm::vec4& getDiffuse();
			lm::vec4& getAmbient();
			lm::vec4& getSpecular();
	};


	class DirectionalLight : public Light
	{
		public:
			DirectionalLight(const lm::vec3& direction = lm::vec3(0, -1, 0), const lm::vec4& diffuse = lm::vec4(1, 1, 1, 0.8f), const lm::vec4& ambient = lm::vec4(1, 1, 1, 0.3f), const lm::vec4& specular = lm::vec4(1, 1, 1, 1));
			lm::vec3& getDirection();

		private:
			lm::vec3 direction;
	};

	class PointLight : public Light
	{
		public:
			PointLight(const lm::vec3& pos = lm::vec3(0, 0, 0), float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f, const lm::vec4& diffuse = lm::vec4(1, 1, 1, 0.8f), const lm::vec4& ambient = lm::vec4(1, 1, 1, 0.3f), const lm::vec4& specular = lm::vec4(1, 1, 1, 1));
			lm::vec3& getPosition();
			float& getConstant();
			float& getLinear();
			float& getQuadratic();

		private:
			lm::vec3 position;
			float constant;
			float linear;
			float quadratic;
	};

	class SpotLight : public Light
	{
		public:
			SpotLight(const lm::vec3& pos = lm::vec3(0, 0, 0), const lm::vec3& direction = lm::vec3(0, 0, -1), float cutOff = 12.5f, float outerCutOff = 17.5f, float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f, const lm::vec4& diffuse = lm::vec4(1, 1, 1, 0.8f), const lm::vec4& ambient = lm::vec4(1, 1, 1, 0.3f), const lm::vec4& specular = lm::vec4(1, 1, 1, 1));
			lm::vec3& getPosition();
			lm::vec3& getDirection();
			float& getCutOff();
			float getCutOffRad();
			float& getOuterCutOff();
			float getOuterCutOffRad();
			float& getConstant();
			float& getLinear();
			float& getQuadratic();

		private:
			lm::vec3 position;
			lm::vec3 direction;
			float cutOff;
			float outerCutOff;
			float constant;
			float linear;
			float quadratic;
	};
}