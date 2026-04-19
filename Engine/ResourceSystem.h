#pragma once
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameObject.h"
#include "Logger.h"



// Чтобы не загружать одну и ту же текстуру несколько раз нужен централизованный склад

namespace XYZengine
{
	class ResourseSystem
	{
	public:
		// загружаем текстуру и сохраняем ее под именем key
		
		bool LoadTexture(const std::string& key, const std::string& path)
		{
			sf::Texture tex;
			if (tex.loadFromFile(path))
			{
				textures[key] = tex;

				Logger::Instance().Info("Texture loaded: " + key + " from " + path); // INFO
				return true;
			}

			Logger::Instance().Error("Failed to load texture: " + path); // ERROR
			return false;
		}


		// получаем ссылку на текстуру по имени

		sf::Texture& GetTexture(const std::string& key)
		{;
		auto it = textures.find(key);
		if (it == textures.end())
		{
			Logger::Instance().Error("Texture key not found: " + key);
			throw std::runtime_error("Texture key not found" + key);
		}
		return it->second;
		}

		// аналогично для звуков

		bool LoadSound(const std::string& key, const std::string& path)
		{
			sf::SoundBuffer buffer;
			if (buffer.loadFromFile(path))
			{
				sounds[key] = buffer;

				Logger::Instance().Info("Sound loaded: " + key + " from " + path); // INFO
				return true;
			}
			return false;

			Logger::Instance().Error("Failed to load sound: " + path); // ERROR
			
		}


		sf::SoundBuffer& GetSound(const std::string& key)
		{
			return sounds[key];
		}

	private:
		std::map<std::string, sf::Texture> textures;
		std::map<std::string, sf::SoundBuffer> sounds;
	};
}