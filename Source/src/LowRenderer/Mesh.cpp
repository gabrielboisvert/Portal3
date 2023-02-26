#include "..\..\include\LowRenderer\Mesh.h"

LowRenderer::Mesh::Mesh(const std::vector<Vertex>& vertex, const std::vector<uint32_t>& indice, const lm::vec4& color) : position(vertex), indices(indice), color(color)
{
    this->VAO.generate(this->position, this->indices, GL_STATIC_DRAW);
}

void LowRenderer::Mesh::Buffer::generate(GLenum bufferType, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    this->bufferType = bufferType;

    glGenBuffers(1, &this->bufferId);
    this->bind(true);
    glBufferData(this->bufferType, size, data, usage);
}

LowRenderer::Mesh::Buffer::~Buffer()
{
    glDeleteBuffers(1, &this->bufferId);
}

void LowRenderer::Mesh::Buffer::bind(bool shouldBind)
{
    if (shouldBind)
        glBindBuffer(this->bufferType, this->bufferId);
    else
        glBindBuffer(this->bufferType, 0);
}

void LowRenderer::Mesh::VertexAttributes::generate(const std::vector<Vertex>& vertex, const std::vector<uint32_t>& indice, GLenum usage)
{
    glGenVertexArrays(1, &this->VAO);
    this->bind(true);
    this->VBO.generate(GL_ARRAY_BUFFER, vertex.size() * sizeof(LowRenderer::Mesh::Vertex), &vertex.front(), usage);
    this->EBO.generate(GL_ELEMENT_ARRAY_BUFFER, indice.size() * sizeof(uint32_t), &indice.front(), usage);

    //Set position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LowRenderer::Mesh::Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    //Set Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LowRenderer::Mesh::Vertex), (void*)(sizeof(lm::vec3)));
    glEnableVertexAttribArray(1);
    //Set texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LowRenderer::Mesh::Vertex), (void*)(2 * sizeof(lm::vec3)));
    glEnableVertexAttribArray(2);

    this->bind(false);
    this->VBO.bind(false);
    this->EBO.bind(false);
}

LowRenderer::Mesh::VertexAttributes::~VertexAttributes()
{
    glDeleteVertexArrays(1, &this->VAO);
}

void LowRenderer::Mesh::VertexAttributes::bind(bool souldBind)
{
    if (souldBind)
        glBindVertexArray(this->VAO);
    else
        glBindVertexArray(0);
}

void LowRenderer::Mesh::draw(Resources::Shader& shader)
{
    shader.setObjColor(this->color);

    this->VAO.bind(true);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    this->VAO.bind(false);
}