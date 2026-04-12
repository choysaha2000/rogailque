#pragma once
#include <SFML/Audio.hpp>
#include "Component.h"


namespace XYZengine
{
	class AudioComponent : public Component {
	public:
		sf::Sound sound;
		sf::Music music;


		// фон музыка
		void PlayMusic(const std::string& path, bool loop = true)
		{
			if (music.openFromFile(path))
			{
				music.setLoop(loop);
				music.play();
			}
		}

		// для коротких звуков через RS

		void PlayEffect(sf::SoundBuffer& buffer)
		{
			sound.setBuffer(buffer);
			sound.play();
		}

	};
}