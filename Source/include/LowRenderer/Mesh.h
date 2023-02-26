#pragma once
#include <LibMaths/Vec3/Vec3.h>
#include <LibMaths/Vec2/Vec2.h>
#include <vector>
#include <glad/glad.h>
#include "../Resources/Shader.h"

namespace LowRenderer
{
	class Mesh
	{
	public:
		struct Vertex
		{
			lm::vec3 position;
			lm::vec3 normal;
			lm::vec2 textureUV;
		};

		Mesh(const std::vector<Vertex>& vertex, const std::vector<uint32_t>& indice, const lm::vec4& color);
		void draw(Resources::Shader& shader);

		private:
			std::vector<Vertex> position;
			std::vector<uint32_t> indices;
			lm::vec4 color;

			class Buffer
			{
			private:
				unsigned int bufferId;
				GLenum bufferType;

			public:
				~Buffer();
				void generate(GLenum bufferType, GLsizeiptr size, const GLvoid* data, GLenum usage);
				void bind(bool shouldBind);
			};

			class VertexAttributes
			{
			private:
				unsigned int VAO;
				Buffer VBO;
				Buffer EBO;


			public:
				~VertexAttributes();
				void generate(const std::vector<Vertex>& vertex, const std::vector<uint32_t>& indice, GLenum usage);
				void bind(bool shouldBind);
			};

			VertexAttributes VAO;
	};
}