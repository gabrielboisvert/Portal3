#pragma once

#include "../Resources/RessourceManager.h"

#include <Vector>
#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../LowRenderer/Mesh.h"

namespace Resources
{
	class Model : public IResource
	{
		public:
			Model(const std::string& filePath);
			~Model();
			void loadModel(const std::string& path);
			void processNode(aiNode* node, const aiScene* scene);
			void processMesh(aiMesh* mesh, const aiScene* scene);
			void draw(Resources::Shader& shader);

			std::string& getFilename();
			std::string& getName();

		private:
			std::string fileName;
			std::string name;
			std::vector<LowRenderer::Mesh*> meshes;
	};
}