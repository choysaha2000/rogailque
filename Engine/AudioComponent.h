#pragma once
#include <SFML/Audio.hpp>
#include "Component.h"
#include <string>
#include "Logger.h"


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
				Logger::Instance().Info("Music started: " + path);
			}

			else
			{
				Logger::Instance().Info("Failed to start music" + path);
			}
		}

		// для коротких звуков через RS

		void PlayEffect(sf::SoundBuffer& buffer)
		{
			sound.setBuffer(buffer);
			sound.play();
			Logger::Instance().Debug("Sound effect played");
		}

	};
}