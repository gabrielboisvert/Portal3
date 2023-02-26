#pragma once

#include "../Core/Debug/Assertion.h"
#include <unordered_map>
#include <string>

namespace Resources
{
	class IResource {};

	class ResourceManager
	{
		private:
			std::unordered_map<std::string, IResource*> manager;

		public:
			~ResourceManager();

			template <typename T, typename ...Args> T* create(const std::string& str, Args... args)
			{
				std::unordered_map<std::string, IResource*>::iterator it = this->manager.find(str);
				if (it != this->manager.end())
					return (T*)it->second;

				T* ressource = new T(str, args...);
				this->manager[str] = ressource;
				return ressource;
			}

			template <typename T> T* get(const std::string& str)
			{
				std::unordered_map<std::string, IResource*>::iterator it = this->manager.find(str);

				ASSERT((it != this->manager.end()));
				if (it == this->manager.end())
					return nullptr;

				return (T*)it->second;
			}

			void Delete(const std::string& str);
	};
}