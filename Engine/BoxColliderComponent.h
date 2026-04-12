#pragma once
#include "SFML/Graphics.hpp"
#include "Component.h"
#include "TransformComponent.h"
#include "GameObject.h"

// Комп описывает прямоугольник, через который нельзя пройти

namespace XYZengine
{
	class  BoxColliderComponent : public Component
	{
	public:
		sf::FloatRect rect; // прямугольник коллизии

		// Инициализируем размер (64х64)
		BoxColliderComponent(float width = 64.0f, float height = 64.0f)
		{
			rect.width = width;
			rect.height = height;
		}

		void Update(float deltaTime) override
		{
			// коллайдер следуют за позицией сущности
			auto* t = owner->GetComponent<TransformComponent>();
			if (t)
			{
				rect.left = t->x;
				rect.top = t->y;
			}
		}
	};
}