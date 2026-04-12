
// Теперь GameObject — это не конкретный игрок или враг,
// а просто пустой контейнер для компонентов.

#pragma once

#include <vector>
#include <memory>
#include <utility>
#include "Component.h"

namespace XYZengine
{
    class GameWorld;

    class GameObject
    {
    public:
        GameWorld* world = nullptr;

        virtual ~GameObject() = default;

        void Init()
        {
            for (auto& c : components)
            {
                c->Init();
            }
        }

        void Update(float deltaTime)
        {
            for (auto& c : components)
            {
                c->Update(deltaTime);
            }
        }

        template <typename T>
        T* GetComponent()
        {
            for (auto& component : components)
            {
                if (auto casted = dynamic_cast<T*>(component.get()))
                {
                    return casted;
                }
            }

            return nullptr;
        }

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            component->owner = this;

            T* raw = component.get();
            components.push_back(std::move(component));

            return *raw;
        }

    private:
        std::vector<std::unique_ptr<Component>> components;
    };
}