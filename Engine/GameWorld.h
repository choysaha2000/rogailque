#pragma once

// Manager is collecting all object in that scene and refresh that ones for every Frame
#include <vector>
#include "GameObject.h"
#include "BoxColliderComponent.h"



namespace XYZengine
{
	class GameWorld {
	public:
		void Update(float deltaTime)
		{
			for (auto obj : objects)
			{
				if (obj)
				{
                            obj->Update(deltaTime);
					}
				
			}
		}

		void AddObject(GameObject* obj) {
			obj->world = this; 
			objects.push_back(obj);
		}
		
		const std::vector<GameObject*>& GetObjects() const
		{
			return objects;
		}



		bool CheckGlobalCollision(const sf::FloatRect& targetRect, GameObject* ignoreObj)
		{
			for (auto obj : objects)
			{

				if (obj == ignoreObj) continue;

		
				auto* collider = obj->GetComponent<BoxColliderComponent>();

				if (collider && collider->blocksMovement)
				{
					if (collider->rect.intersects(targetRect))
					{
						return true;
					}
				}
			}
			return false;
		}

		// clear old stuff

		void Clear()
		{
			for (auto obj : objects)
			{
                    delete obj;
		}

			objects.clear();
		}


	private:
		std::vector <GameObject*> objects;

	};


}