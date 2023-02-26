#pragma once
#include "RessourceManager.h"
#include <string>

namespace Resources
{
	class Texture : public IResource
	{
		public:
			Texture(const std::string& fileName);
			~Texture();
			void activate(bool shouldActivate);
			unsigned int getTexture();
			std::string& getFileName();
			std::string& getName();

		private:
			std::string name;
			std::string fileName;
			unsigned int texture = 0;
	};
}