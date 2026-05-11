#pragma once
// enemy is following to player
#include "Component.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include <cmath>
#include "Logger.h"

namespace XYZengine
{
	class EnemyFollowComponent : public Component
	{
	public:
		GameObject* target = nullptr; // object of an hunting
		float speed = 100.0f;

		void Update(float deltaTime) override
		{
			if (!target)
			{
				Logger::Instance().Warning("EnemyFollowComponent: target is null");
				return;
			}
			auto* myTrans = owner->GetComponent<TransformComponent>();
			auto* targetTrans = target->GetComponent<TransformComponent>();

			if (myTrans && targetTrans)
			{
				// vector to player
				float dx = targetTrans->x - myTrans->x;
				float dy = targetTrans->y - myTrans->y;

				// cannot move faster on diaganal 

				float lenght = sqrt(dx * dx + dy * dy);
				if (lenght > 1.0)
				{
					myTrans->x += (dx / lenght) * speed * deltaTime;
					myTrans->y += (dy / lenght) * speed * deltaTime;
					//Logger::Instance().Debug("Debug is moving toward player");
				}

			}

			if (!myTrans || !targetTrans)
			{
				Logger::Instance().Warning("EnemyFollowComponent: missing transform");
				return;
			}
		}
	};
}