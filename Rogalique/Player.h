#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <vector>
#include "GameObject.h"

class Game;

class Paddle : public GameObject
{
public:
    Paddle();


    void SetWidthMultiplier(float m) {
        shape.setSize({ SETTINGS.PADDLE_WIDTH * m, SETTINGS.PADDLE_HEIGHT });
        shape.setOrigin(SETTINGS.PADDLE_WIDTH * m / 2.f, SETTINGS.PADDLE_HEIGHT / 2.f);
    }
   
    void Init(Game& game) override;
    void Update(float deltaTime, Game& game, sf::RenderWindow& window) override;
    void Draw(sf::RenderWindow& window) override;

    sf::FloatRect GetBounds() const;
    sf::Vector2f GetPosition() const;

private:
    sf::RectangleShape shape;
    float speed;
};


class Ball: public GameObject
{
public:

    void SetSpeedMultiplier(float m)
    {
        velocity = sf::Vector2f(
            (velocity.x > 0 ? 1 : -1) * speed * m * 0.707f,
            (velocity.y > 0 ? 1 : -1) * speed * m * 0.707f
        );
    }


    void SetPiersing(bool val) { isPiersingFlag = val;}
    bool IsPiersing() const { return isPiersingFlag; }


    Ball();
    void Init(Game& game) override;
    void Update(float deltaTime, Game& game, sf::RenderWindow& window) override;
    void Draw(sf::RenderWindow& window) override;


    void BounceX();
    void BounceY();
    sf::Vector2f GetVelocity() const;

    sf::FloatRect GetBounds() const;
    sf::Vector2f GetPosition() const;

private:
    bool isPiersingFlag = false;

    sf::CircleShape shape;
    sf::Vector2f velocity;
    float speed;
};