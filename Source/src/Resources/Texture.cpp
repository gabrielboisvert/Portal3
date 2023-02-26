#include "../../include/Resources/Texture.h"
#include "../../include/Core/Debug/Log.h"
#include "fstream"
#include "sstream"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Resources::Texture::Texture(const std::string& fileName) : fileName(fileName)
{
	int count = this->fileName.find_last_of('/') + 1;
	this->name = this->fileName.substr(count, this->fileName.size());

	count = this->name.find_last_of('.');
	this->name = this->name.substr(0, count);

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else
	{
		Core::Log::print("Failed to send texture to opengl\n");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.f);
	glBindTextureUnit(0, this->texture);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Resources::Texture::~Texture()
{
	glDeleteTextures(GL_TEXTURE_2D, &this->texture);
}

void Resources::Texture::activate(bool shouldActivate)
{
	if (shouldActivate)
	{
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glBindTextureUnit(GL_TEXTURE0, this->texture);
	}
	else
	{
		glBindTextureUnit(GL_TEXTURE0, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

unsigned int Resources::Texture::getTexture()
{
	return this->texture;
}

std::string& Resources::Texture::getFileName()
{
	return this->fileName;
}

std::string& Resources::Texture::getName()
{
	return this->name;
}
