#include "../../include/Sound/SoundEngine.h"

Sound::SoundEngine::SoundEngine()
{
	this->soundEngine = irrklang::createIrrKlangDevice();
	this->soundEngine->setDefault3DSoundMinDistance(5);
}

Sound::SoundEngine::~SoundEngine()
{
	if (this->soundEngine != nullptr)
		this->soundEngine->drop();
}

irrklang::ISoundEngine* Sound::SoundEngine::getEngine()
{
	return this->soundEngine;
}

void Sound::SoundEngine::playSound3D(const std::string& soundPath, const lm::vec3& position, float minDist)
{
	this->soundEngine->play3D(soundPath.c_str(), irrklang::vec3df(position.X(), position.Y(), position.Z()));
}

void Sound::SoundEngine::playSound2D(const std::string& soundPath)
{
	this->soundEngine->play2D(soundPath.c_str());
}

irrklang::ISound* Sound::SoundEngine::playMusic3D(const std::string& soundPath, const lm::vec3& position, bool looped, bool startPaused, bool tracked)
{
	return this->soundEngine->play3D(soundPath.c_str(), irrklang::vec3df(position.X(), position.Y(), position.Z()), looped, startPaused, tracked);
}
