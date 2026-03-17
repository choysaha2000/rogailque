#include "Bonus.h"
#include "Game.h"


void Bonus::Update(float deltaTime, Game& game, sf::RenderWindow& window)
{
	if (IsCollected()) return;

	// падаем вниз

	shape.move(velocity * deltaTime);

	// вышли за нижнюю границу - удаляемся

	if (shape.getPosition().y > SETTINGS.SCREEN_HEIGHT + 20.f)
		m_isCollected = true; // переиспользуем флаг как мертв



	// проверяем столкновение с платформой
	if (GetBounds().intersects(game.GetPaddle().GetBounds()))
	{
		Collect();

		// Применяем эффект к мячу и платформе

		if (effect)
		{
			effect->ApplyToBall(game.GetBall());
			effect->ApplyToPaddle(game.GetPaddle());

			// запускаем таймер эффекта 

			game.ActivateBonus(std::move(effect), 5.f);
		}
	}
}