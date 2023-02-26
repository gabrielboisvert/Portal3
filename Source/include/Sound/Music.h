#include "../../include/Sound/SoundEngine.h"
#include <LibMaths/Vec3/Vec3.h>
#include <vector>

namespace Sound
{
	class Music {

	private:
		SoundEngine* soundEngine = nullptr;
		irrklang::ISound* music = nullptr;

		lm::vec3& position;
		std::vector<std::string> musics;
		int current = 0;

	public:
		Music(lm::vec3& position, SoundEngine& soundEngine);
		~Music();
		void addMusic(const std::string& music);
		void Sound::Music::removeMusic(std::vector<std::string>::iterator& it);
		void playMusic();
		void update();
		std::vector<std::string>& getPlaylist();
	};
}