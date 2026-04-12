#pragma once
#include <SFML/Window/Keyboard.hpp>
#include "Component.h"
#include "TransformComponent.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "GameWorld.h"

// компонент принимающей команды клавы и меняющий координаты в TransFormCMP

namespace XYZengine
{
	class InputComponent : public Component
	{
	public:

		float speed = 200.0f; // скорость движения

        void Update(float deltaTime) override
        {
            auto* transform = owner->GetComponent<TransformComponent>();
            auto* myCollider = owner->GetComponent<BoxColliderComponent>();

            if (!transform || !myCollider || !owner->world)
            {
                std::cout << "Input blocked: missing transform/collider/world\n";
                return;
            }

            std::cout << "InputComponent update working\n";

            float moveX = 0;
            float moveY = 0;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveY -= speed * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveY += speed * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveX -= speed * deltaTime;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveX += speed * deltaTime;

            std::cout << "moveX = " << moveX << ", moveY = " << moveY << "\n";

            sf::FloatRect nextRectX = myCollider->rect;
            nextRectX.left += moveX;
            if (!owner->world->CheckGlobalCollision(nextRectX, owner))
            {
                transform->x += moveX;
            }

            sf::FloatRect nextRectY = myCollider->rect;
            nextRectY.top += moveY;
            if (!owner->world->CheckGlobalCollision(nextRectY, owner))
            {
                transform->y += moveY;
            }
        }


	};


}