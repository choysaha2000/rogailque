#pragma once
#include "GameObject.h"
#include "BonusStrategy.h"
#include <memory>

// объект бонуса падает вниз после уничтожения блока

class Bonus : public GameObject
{
public:
	Bonus() = default;

	void Init(Game& game) override{}

	// создать бонус в позиции блока 

	void Spawn(sf::Vector2f position, std::unique_ptr<IBonusEffect>effectArg)

	{

        this->effect = std::move(effectArg);
        shape.setRadius(12.f);
        shape.setOrigin(12.f, 12.f);
        shape.setPosition(position);
        shape.setFillColor(this->effect->GetColor());
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(2.f);

        
        m_isCollected = false;
        velocity = { 0.f, 150.f }; // падает вниз
	}

    void Update(float deltaTime, Game& game, sf::RenderWindow& window) override;

    void Draw(sf::RenderWindow& window) override
    {
        if (!m_isCollected) window.draw(shape);
    }

    sf::FloatRect GetBounds() const { return shape.getGlobalBounds(); }
    bool IsCollected() const { return m_isCollected; }
    void Collect() { m_isCollected = true; }
    IBonusEffect* GetEffect() { return effect.get(); }

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    std::unique_ptr <IBonusEffect> effect;
    bool m_isCollected = false;

};