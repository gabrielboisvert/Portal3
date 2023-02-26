#pragma once
#include "../LowRenderer/Light.h"
#include "RessourceManager.h"
#include "glad/glad.h"
#include <filesystem>
#include "../LowRenderer/Light.h"

namespace Resources
{
	class Shader : public IResource
	{
		private:
			std::string name;
			std::string vertexFilename;
			std::string fragFilename;

			int shaderProgram = -1;
			int vertexShader = -1;
			int fragmentShader = -1;

			std::vector<LowRenderer::DirectionalLight*> dirLights;
			std::vector<LowRenderer::PointLight*> pointLights;
			std::vector<LowRenderer::SpotLight*> spotLights;

		public:
			Shader(const std::string& str, const std::string& vertex, const std::string& fragment);
			~Shader();
			bool setVertexShader(const std::string& filename);
			bool setFragmentShader(const std::string& filename);

			void addDirLights(LowRenderer::DirectionalLight* light);
			void removeDirLights(LowRenderer::DirectionalLight* light);

			void addPointLights(LowRenderer::PointLight* light);
			void removePointLights(LowRenderer::PointLight* light);

			void addSpotLights(LowRenderer::SpotLight* light);
			void removeSpotLights(LowRenderer::SpotLight* light);

			void setObjColor(lm::vec4& color);
			const int& getShaderProgram() const;
			std::string& getName();
			bool link();
			void activate(bool shouldActivate);

			void setUniform1i(const std::string& str, const GLint data) const;
			void setUniform1f(const std::string& str, const GLfloat data) const;
			void setUniformMat3(const std::string& str, const GLfloat* data) const;
			void setUniformMat4(const std::string& str, const GLfloat* data) const;
			void setUniformVec3(const std::string& str, const GLfloat* data) const;
			void setUniformVec4(const std::string& str, const GLfloat* data) const;

			void clearLights();

			std::string& getVertexFileName();
			std::string& getFragmentFileName();
	};
}