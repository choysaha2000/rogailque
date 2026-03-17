#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Player.h"

// интерфейс бонуса (стратеги€ поведени€)

class IBonusEffect
{
public:
    virtual ~IBonusEffect() = default;
    virtual void ApplyToBall(Ball& ball) = 0;
    virtual void ApplyToPaddle(Paddle& paddle) = 0;
    virtual void Remove(Ball& ball, Paddle& paddle) = 0;
    virtual std::string GetName() const = 0;
    virtual sf::Color GetColor() const = 0;
};

// огненный м€ч

class FireBallEffect : public IBonusEffect
{
public: 
    void ApplyToBall(Ball& ball) override
    {
        ball.SetSpeedMultiplier(2.0f);  
        ball.SetPiersing(true);
    };
    void ApplyToPaddle(Paddle& paddle) override {};
    void Remove(Ball& ball, Paddle& paddle) override {
        
        ball.SetSpeedMultiplier(1.0f);
        ball.SetPiersing(false);
    };
    std::string GetName() const override { return "FireBall"; }
    sf::Color GetColor() const override { return sf::Color::Red; }

};

// Ѕольша€ платформа увеличавает размер

class WidePaddleEffect : public IBonusEffect
{
public:
    void ApplyToBall (Ball& ball) override {}
    void ApplyToPaddle(Paddle& paddle) override
    {
        paddle.SetWidthMultiplier(2.0f);
    }

    void Remove(Ball& ball, Paddle& paddle) override
    {
        paddle.SetWidthMultiplier(1.0f);
    }

    std::string GetName() const override { return "WidePaddle"; }
    sf::Color GetColor() const override { return sf::Color::Cyan; }

};

// ћедленный м€ч замедл€ет

class SlowBallEffect : public IBonusEffect
{
public:
    void ApplyToBall(Ball& ball) override
    {
        ball.SetSpeedMultiplier(0.5f);
    }

    void ApplyToPaddle (Paddle& paddle) override {}
    void Remove( Ball & ball, Paddle & paddle ) override
    {
        ball.SetSpeedMultiplier(1.0f);
    }

    std::string GetName() const override { return "SlowBall"; }
    sf::Color GetColor() const override { return sf::Color::Blue; }
};