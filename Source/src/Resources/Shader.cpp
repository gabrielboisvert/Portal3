#include "../../include/Resources/Shader.h"
#include "../../include/Core/Debug/Log.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>

Resources::Shader::Shader(const std::string& str, const std::string& vertex, const std::string& fragment) : name(str), vertexFilename(vertex), fragFilename(fragment)
{
	if (this->setVertexShader(vertex))
		if (this->setFragmentShader(fragment))
			this->link();
}

Resources::Shader::~Shader()
{
	if (this->vertexShader != -1)
		glDeleteShader(this->vertexShader);
	
	if (this->fragmentShader != -1)
		glDeleteShader(this->fragmentShader);

	if (this->shaderProgram != -1)
		glDeleteShader(this->shaderProgram);
}

bool Resources::Shader::setVertexShader(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		Core::Log::print("Can not open file %s\n", filename.c_str());
		return false;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	std::string str = buffer.str();
	const char* vertexShaderSource = str.c_str();

	// vertex shader
	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(this->vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(this->vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(this->vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(this->vertexShader, 512, NULL, infoLog);
		Core::Log::print("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n", infoLog);
		return false;
	}
	return true;
}

bool Resources::Shader::setFragmentShader(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		Core::Log::print("Can not open file %s\n", filename.c_str());
		return false;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	std::string str = buffer.str();
	const char* fragmentShaderSource = str.c_str();

	//fragment shader
	this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(this->fragmentShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(this->fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(this->fragmentShader, 512, NULL, infoLog);
		Core::Log::print("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED %s\n", infoLog);
		return false;
	}
	return true;
}

void Resources::Shader::addDirLights(LowRenderer::DirectionalLight* light)
{
	this->dirLights.push_back(light);
}

void Resources::Shader::removeDirLights(LowRenderer::DirectionalLight* light)
{
	std::vector<LowRenderer::DirectionalLight*>::iterator it = std::find(this->dirLights.begin(), this->dirLights.end(), light);
	if (it != this->dirLights.end())
		this->dirLights.erase(it);
}

void Resources::Shader::addPointLights(LowRenderer::PointLight* light)
{
	this->pointLights.push_back(light);
}

void Resources::Shader::removePointLights(LowRenderer::PointLight* light)
{
	std::vector<LowRenderer::PointLight*>::iterator it = std::find(this->pointLights.begin(), this->pointLights.end(), light);
	if (it != this->pointLights.end())
		this->pointLights.erase(it);
}

void Resources::Shader::addSpotLights(LowRenderer::SpotLight* light)
{
	this->spotLights.push_back(light);
}

void Resources::Shader::removeSpotLights(LowRenderer::SpotLight* light)
{
	std::vector<LowRenderer::SpotLight*>::iterator it = std::find(this->spotLights.begin(), this->spotLights.end(), light);
	if (it != this->spotLights.end())
		this->spotLights.erase(it);
}

void Resources::Shader::setObjColor(lm::vec4& color)
{
	this->setUniformVec4("objColor", &color[0]);
}

const int& Resources::Shader::getShaderProgram() const
{
	return this->shaderProgram;
}

std::string& Resources::Shader::getName()
{
	return this->name;
}

bool Resources::Shader::link()
{
	// link shaders
	this->shaderProgram = glCreateProgram();
	glAttachShader(this->shaderProgram, this->vertexShader);
	glAttachShader(this->shaderProgram, this->fragmentShader);
	glLinkProgram(this->shaderProgram);

	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(this->shaderProgram, 512, NULL, infoLog);
		Core::Log::print("ERROR::SHADER::PROGRAM::LINKING_FAILED %s\n", infoLog);
		return false;
	}

	glDeleteShader(this->vertexShader);
	this->vertexShader = -1;

	glDeleteShader(this->fragmentShader);
	this->fragmentShader = -1;

	return true;
}

void Resources::Shader::activate(bool shouldActivate)
{
	if (!shouldActivate)
	{
		glUseProgram(0);
		return;
	}

	glUseProgram(this->shaderProgram);

	int transformLoc = glGetUniformLocation(this->shaderProgram, "dirlightCount");
	glUniform1i(transformLoc, this->dirLights.size());

	//Send directionnal light
	for (unsigned int i = 0; i < this->dirLights.size(); i++)
	{
		std::string attribut = "dirLights[" + std::to_string(i) + "].";

		LowRenderer::DirectionalLight& dir = *this->dirLights[i];
		this->setUniformVec4(attribut + "ambient", &dir.getAmbient()[0]);
		this->setUniformVec4(attribut + "diffuse", &dir.getDiffuse()[0]);
		this->setUniformVec4(attribut + "specular", &dir.getSpecular()[0]);

		this->setUniformVec3(attribut + "direction", &dir.getDirection()[0]);
	}


	transformLoc = glGetUniformLocation(this->shaderProgram, "pointlightCount");
	glUniform1i(transformLoc, this->pointLights.size());

	//Send point light
	for (unsigned int i = 0; i < this->pointLights.size(); i++)
	{
		std::string attribut = "pointLights[" + std::to_string(i) + "].";

		LowRenderer::PointLight& dir = *this->pointLights[i];
		this->setUniformVec4(attribut + "ambient", &dir.getAmbient()[0]);
		this->setUniformVec4(attribut + "diffuse", &dir.getDiffuse()[0]);
		this->setUniformVec4(attribut + "specular", &dir.getSpecular()[0]);

		this->setUniformVec3(attribut + "position", &dir.getPosition()[0]);

		this->setUniform1f(attribut + "constant", dir.getConstant());
		this->setUniform1f(attribut + "linear", dir.getLinear());
		this->setUniform1f(attribut + "quadratic", dir.getQuadratic());
	}


	transformLoc = glGetUniformLocation(this->shaderProgram, "spotlightCount");
	glUniform1i(transformLoc, this->spotLights.size());

	//Send point light
	for (unsigned int i = 0; i < this->spotLights.size(); i++)
	{
		std::string attribut = "spotLights[" + std::to_string(i) + "].";

		LowRenderer::SpotLight& dir = *this->spotLights[i];
		this->setUniformVec4(attribut + "ambient", &dir.getAmbient()[0]);
		this->setUniformVec4(attribut + "diffuse", &dir.getDiffuse()[0]);
		this->setUniformVec4(attribut + "specular", &dir.getSpecular()[0]);

		this->setUniformVec3(attribut + "position", &dir.getPosition()[0]);
		this->setUniformVec3(attribut + "direction", &dir.getDirection()[0]);
		this->setUniform1f(attribut + "cutOff", dir.getCutOffRad());
		this->setUniform1f(attribut + "outerCutOff", dir.getOuterCutOffRad());

		this->setUniform1f(attribut + "constant", dir.getConstant());
		this->setUniform1f(attribut + "linear", dir.getLinear());
		this->setUniform1f(attribut + "quadratic", dir.getQuadratic());
	}
}

void Resources::Shader::setUniform1i(const std::string& str, const GLint data) const
{
	int transformLoc = glGetUniformLocation(this->shaderProgram, str.c_str());
	if (transformLoc == -1)
		return;

	glUniform1i(transformLoc, data);
}


void Resources::Shader::setUniform1f(const std::string& str, const GLfloat data) const
{
	int transformLoc = glGetUniformLocation(this->shaderProgram, str.c_str());
	if (transformLoc == -1)
		return;

	glUniform1f(transformLoc, data);
}

void Resources::Shader::setUniformMat3(const std::string& str, const GLfloat* data) const
{
	int transformLoc = glGetUniformLocation(this->shaderProgram, str.c_str());
	if (transformLoc == -1)
		return;

	glUniformMatrix3fv(transformLoc, 1, GL_FALSE, data);
}

void Resources::Shader::setUniformMat4(const std::string& str, const GLfloat* data) const
{
	int transformLoc = glGetUniformLocation(this->shaderProgram, str.c_str());
	if (transformLoc == -1)
		return;

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, data);
}

void Resources::Shader::setUniformVec3(const std::string& str, const GLfloat* data) const
{
	int transformLoc = glGetUniformLocation(this->shaderProgram, str.c_str());
	if (transformLoc == -1)
		return;

	glUniform3fv(transformLoc, 1, data);
}

void Resources::Shader::setUniformVec4(const std::string& str, const GLfloat* data) const
{
	int transformLoc = glGetUniformLocation(this->shaderProgram, str.c_str());
	if (transformLoc == -1)
		return;

	glUniform4fv(transformLoc, 1, data);
}

std::string& Resources::Shader::getVertexFileName()
{
	return this->vertexFilename;
}

std::string& Resources::Shader::getFragmentFileName()
{
	return this->fragFilename;
}

void Resources::Shader::clearLights()
{
	this->dirLights.clear();
	this->pointLights.clear();
	this->spotLights.clear();
}
