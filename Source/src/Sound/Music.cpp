#include "../../include/Sound/Music.h"

Sound::Music::Music(lm::vec3& position, SoundEngine& soundEngine) : position(position), soundEngine(&soundEngine)
{
}

Sound::Music::~Music()
{
	if (this->music != nullptr)
	{
		this->music->stop();
		this->music->drop();
	}
}

void Sound::Music::addMusic(const std::string& music)
{
	if (std::find(this->musics.begin(), this->musics.end(), music) != this->musics.end())
		return;

	this->musics.push_back(music);
}

void Sound::Music::removeMusic(std::vector<std::string>::iterator& it)
{
	if ((*it) == this->musics[this->current])
		this->music->stop();

	this->musics.erase(it);
	if (this->musics.size() == 0)
	{
		this->music->drop();
		this->music = nullptr;
	}
	this->current = 0;
}

void Sound::Music::playMusic()
{
	if (this->musics.size() == 0)
		return;

	if (this->music != nullptr)
		this->music->drop();
	
	this->music = this->soundEngine->playMusic3D(this->musics[this->current].c_str(), this->position);
	this->music->setMinDistance(1.5f);
}

void Sound::Music::update()
{
	if (this->music == nullptr)
	{
		this->playMusic();
		return;
	}

	if (this->music->isFinished())
	{
		this->current = (this->current + 1) % musics.size();
		this->playMusic();
	}
}

std::vector<std::string>& Sound::Music::getPlaylist()
{
	return this->musics;
}
