#pragma once
#include "SFML/Graphics.hpp"
#include "Component.h"
#include "TransformComponent.h"
#include "GameObject.h"


// Drawing square with 4 angles that cannot be crossed.
namespace XYZengine
{
	class  BoxColliderComponent : public Component
	{
	public:
		sf::FloatRect rect; // square collision
		bool blocksMovement = true;

		// Initialize of size (64x64)
		BoxColliderComponent(float width = 64.0f, float height = 64.0f, bool blocks = true)
		{
			rect.width = width;
			rect.height = height;
			blocksMovement = blocks;
		}

		void Update(float deltaTime) override
		{
			// Collider is moving for object 
			auto* t = owner->GetComponent<TransformComponent>();
			if (t)
			{
				rect.left = t->x;
				rect.top = t->y;
			}
		}
	};
}