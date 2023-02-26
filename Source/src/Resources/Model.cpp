#include "..\..\include\Resources\Model.h"
#include "..\..\include\Core\Debug\Log.h"

#include <assimp/Importer.hpp>



Resources::Model::Model(const std::string& filePath) : fileName(filePath)
{
    int count = this->fileName.find_last_of('/') + 1;
    this->name = this->fileName.substr(count, this->fileName.size());

    count = this->name.find_last_of('.');
    this->name = this->name.substr(0, count);

    this->loadModel(filePath);
}

Resources::Model::~Model()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        delete meshes[i];
}

void Resources::Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType |  aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals | aiProcess_GlobalScale);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Core::Log::print("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        return;
    }

    this->processNode(scene->mRootNode, scene);
}

void Resources::Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        this->processNode(node->mChildren[i], scene);
}

void Resources::Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<LowRenderer::Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    //vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        LowRenderer::Mesh::Vertex vertex;
        lm::vec3 vector;
        vector.X() = mesh->mVertices[i].x;
        vector.Y() = mesh->mVertices[i].y;
        vector.Z() = mesh->mVertices[i].z;
        vertex.position = vector;
        
        // normals
        if (mesh->HasNormals())
        {
            vector.X() = mesh->mNormals[i].x;
            vector.Y() = mesh->mNormals[i].y;
            vector.Z() = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        
        // texture coordinates
        if (mesh->mTextureCoords[0])
        {
            lm::vec2 vec;
            vec.X() = mesh->mTextureCoords[0][i].x;
            vec.Y() = mesh->mTextureCoords[0][i].y;
            vertex.textureUV = vec;
        }

        vertices.push_back(vertex);
    }

    //faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Color
    aiColor4D diffuse;
    lm::vec4 color = lm::vec4::unitVal;
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        color = lm::vec4(diffuse.r, diffuse.g, diffuse.b, 1);

    this->meshes.emplace_back(new LowRenderer::Mesh(vertices, indices, color));
}

void Resources::Model::draw(Resources::Shader& shader)
{
    for (unsigned int i = 0; i < this->meshes.size(); i++)
        this->meshes[i]->draw(shader);
}

std::string& Resources::Model::getFilename()
{
    return this->fileName;
}

std::string& Resources::Model::getName()
{
    return this->name;
}
