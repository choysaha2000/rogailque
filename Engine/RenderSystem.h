#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "SpriteRenderComponent.h"

namespace XYZengine
{
    class RenderSystem
    {
    public:
        void Render(sf::RenderWindow& window, const std::vector<GameObject*>& objects)
        {
            for (auto obj : objects)
            {
                auto* renderer = obj->GetComponent<SpriteRenderComponent>();
                if (renderer)
                {
                    renderer->Draw(window);
                }
            }
        }
    };
}