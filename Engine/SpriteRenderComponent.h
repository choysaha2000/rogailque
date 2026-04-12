#pragma once
// компонент связывает текстуру из ResourceSystem и матрицу из TransformComp


#include <SFML/Graphics.hpp>
#include "Component.h"
#include "TransformComponent.h"

namespace XYZengine
{
	class SpriteRenderComponent : public Component
	{
	public: 
		sf::Sprite sprite;

		// метод отрисовки, который будет вызываться сис рендеринга

		void Draw(sf::RenderWindow& window)
		{
			// матрица из TFC

			auto* transform = owner->GetComponent<TransformComponent>();


			if (transform)
			{
				// передаем координаты из transform

				sprite.setPosition(transform->x, transform->y);
				sprite.setRotation(transform->rotation);
			}

			window.draw(sprite);
			
		}
	};
}