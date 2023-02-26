#include "..\..\include\LowRenderer\Light.h"

lm::vec4& LowRenderer::Light::getDiffuse()
{
	return this->diffuse;
}

LowRenderer::Light::Light(const lm::vec4& diffuse, const lm::vec4& ambient, const lm::vec4& specular) : diffuse(diffuse), ambient(ambient), specular(specular)
{
}

lm::vec4& LowRenderer::Light::getAmbient()
{
	return this->ambient;
}

lm::vec4& LowRenderer::Light::getSpecular()
{
	return this->specular;
}


LowRenderer::DirectionalLight::DirectionalLight(const lm::vec3& direction, const lm::vec4& diffuse, const lm::vec4& ambient, const lm::vec4& specular) : Light(diffuse, ambient, specular), direction(direction.normalized()) {}

lm::vec3& LowRenderer::DirectionalLight::getDirection()
{
	return this->direction;
}

LowRenderer::PointLight::PointLight(const lm::vec3& pos, float constant, float linear, float quadratic, const lm::vec4& diffuse, const lm::vec4& ambient, const lm::vec4& specular) : Light(diffuse, ambient, specular), position(pos), constant(constant), linear(linear), quadratic(quadratic) {}

lm::vec3& LowRenderer::PointLight::getPosition()
{
	return this->position;
}

float& LowRenderer::PointLight::getConstant()
{
	return this->constant;
}

float& LowRenderer::PointLight::getLinear()
{
	return this->linear;
}

float& LowRenderer::PointLight::getQuadratic()
{
	return this->quadratic;
}

LowRenderer::SpotLight::SpotLight(const lm::vec3& pos, const lm::vec3& direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic, const lm::vec4& diffuse, const lm::vec4& ambient, const lm::vec4& specular) : Light(diffuse, ambient, specular), position(pos), direction(direction), cutOff(cutOff), outerCutOff(outerCutOff), constant(constant), linear(linear), quadratic(quadratic) {}

lm::vec3& LowRenderer::SpotLight::getPosition()
{
	return this->position;
}

lm::vec3& LowRenderer::SpotLight::getDirection()
{
	return this->direction;
}

float& LowRenderer::SpotLight::getCutOff()
{
	return this->cutOff;
}

float LowRenderer::SpotLight::getCutOffRad()
{
	return std::cos(lm::vec3::degreesToRadians(this->cutOff));
}

float& LowRenderer::SpotLight::getOuterCutOff()
{
	return this->outerCutOff;
}

float LowRenderer::SpotLight::getOuterCutOffRad()
{
	return std::cos(lm::vec3::degreesToRadians(this->outerCutOff));
}

float& LowRenderer::SpotLight::getConstant()
{
	return this->constant;
}

float& LowRenderer::SpotLight::getLinear()
{
	return this->linear;
}

float& LowRenderer::SpotLight::getQuadratic()
{
	return this->quadratic;
}
