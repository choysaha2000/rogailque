#pragma once

// CMP is collecting textures from ResourceSystem and Matrix from TransformCMP

#include <SFML/Graphics.hpp>
#include "Component.h"
#include "TransformComponent.h"

namespace XYZengine
{
	class SpriteRenderComponent : public Component
	{
	public: 
		sf::Sprite sprite;


		// Method of drawing, that ones would be calling sysRender

		void Draw(sf::RenderWindow& window)
		{
			// matrix from TFC

			auto* transform = owner->GetComponent<TransformComponent>();


			if (transform)
			{
	
				// give coordinates from Transform

				sprite.setPosition(transform->x, transform->y);
				sprite.setRotation(transform->rotation);
			}

			window.draw(sprite);
			
		}
	};
}