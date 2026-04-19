#pragma once
// комп преследовани€. враг знает где игрок и т€нет свои координаты к нему
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
		GameObject* target = nullptr; // объект охоты
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
				// вектор направлени€ к цели
				float dx = targetTrans->x - myTrans->x;
				float dy = targetTrans->y - myTrans->y;

				// чтобы не летел быстрее по диагонали

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