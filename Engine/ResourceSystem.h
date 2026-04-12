#pragma once
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameObject.h"



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
				return true;
			}
			return false;
		}


		// получаем ссылку на текстуру по имени

		sf::Texture& GetTexture(const std::string& key)
		{
			return textures[key];
		}

		// аналогично для звуков

		bool LoadSound(const std::string& key, const std::string& path)
		{
			sf::SoundBuffer buffer;
			if (buffer.loadFromFile(path))
			{
				sounds[key] = buffer;
				return true;
			}
			return false;
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