#include "..\..\include\Resources\RessourceManager.h"

Resources::ResourceManager::~ResourceManager()
{
	for (std::unordered_map<std::string, IResource*>::iterator it = this->manager.begin(); it != this->manager.end(); it++)
		delete it->second;
	this->manager.clear();
}

void Resources::ResourceManager::Delete(const std::string& str)
{
	std::unordered_map<std::string, IResource*>::iterator it = this->manager.find(str);
	if (it != this->manager.end())
	{
		delete it->second;
		this->manager.erase(it);
	}
}