#pragma once

#include "Component.h"
#include "Logger.h"
#include <algorithm>


namespace XYZengine
{
	class HealthComponent : public Component
	{
	public:
		int health = 100;
		int armor = 0;

		HealthComponent(int hp = 100, int arm = 0)
			: health(hp), armor(arm)
		{
		}

		void TakeDamage(int damage)
		{
			if (damage <= 0)
			{
				Logger::Instance().Warning("HealthComponent: non-positive damage ignored");
				return;
			}

			int remainingDamage = damage;

			if (armor > 0)
			{
				int absorbed = std::min(armor, remainingDamage);
				armor -= absorbed;
				remainingDamage -= absorbed;

				Logger::Instance().Info("Armor absorbed damage: " + std::to_string(absorbed));
			}

			if (remainingDamage > 0)
			{
				health -= remainingDamage;
				Logger::Instance().Info("Health reduced by: " + std::to_string(remainingDamage));
			}


			if (health < 0)
			{
				health = 0;
			}

			Logger::Instance().Debug("Current HP: " + std::to_string(health) + ", Armor: " + std::to_string(armor));


		};


		bool IsDead() const
		{
			return health <= 0;
		}
	};
}