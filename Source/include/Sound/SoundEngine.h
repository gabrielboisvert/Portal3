#pragma once
#include <iostream>
#include <stdio.h>
#include <irrKlang.h>
#include <LibMaths/Vec3/Vec3.h>

#pragma comment(lib, "irrKlang.lib")

namespace Sound
{

	class SoundEngine
	{
	private:
		irrklang::ISoundEngine* soundEngine = nullptr;

	public:
		SoundEngine();
		~SoundEngine();
		irrklang::ISoundEngine* getEngine();

		void playSound3D(const std::string& soundPath, const lm::vec3& position, float minDist = 1);
		void playSound2D(const std::string& soundPath);
		irrklang::ISound* playMusic3D(const std::string& soundPath, const lm::vec3& position, bool looped = false, bool startPaused = false, bool tracked = true);
	};
}