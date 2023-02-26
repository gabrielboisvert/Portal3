#pragma once
#include <vector>

namespace Core
{
	template <class T> class Scene
	{
	public:
		T* addNode(T* node)
		{
			this->gameObject.push_back(node);
			return this->gameObject.back();
		}

		void removeNode(T& node)
		{
			std::vector<T*>::iterator it = std::find(gameObject.begin(), gameObject.end(), &node);
			if (it != gameObject.end())
				gameObject.erase(it);
		}

		void draw()
		{
			for (unsigned int i = 0; i < this->gameObject.size(); i++)
				this->gameObject[i]->draw();
		}
		void update(float delta)
		{
			for (unsigned int i = 0; i < this->gameObject.size(); i++)
				this->gameObject[i]->update(delta);
		}

		std::vector<T*>& getGameObjects()
		{
			return gameObject;
		}

		~Scene()
		{
			for (unsigned int i = 0; i < this->gameObject.size(); i++)
				delete this->gameObject[i];
			this->gameObject.clear();
		}

		void clear()
		{
			for (unsigned int i = 0; i < this->gameObject.size(); i++)
				delete this->gameObject[i];
			this->gameObject.clear();
		}


	private:
		std::vector<T*> gameObject;
	};
}