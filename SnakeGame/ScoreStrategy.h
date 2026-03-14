#pragma once
#include <string>

// базовая стратегия начисления очков

class IScoreStrategy
{
public:
    virtual ~IScoreStrategy() = default;
    virtual int PointsForBrickHit() = 0;
    virtual int PointsForBrickDestroyed() = 0;
    virtual int PointsForPaddleHit() = 0;
    virtual std::string GetName() = 0;
};


// easy 

class EasyScoreStrategy : public IScoreStrategy
{
public:
    int PointsForBrickHit() override { return 5; }
    int PointsForBrickDestroyed() override { return 25; }
    int PointsForPaddleHit() override { return 2; }
    std::string GetName() override { return "Easy"; }
};

// classic

class ClassicScoreStrategy : public IScoreStrategy
{
public:
    int PointsForBrickHit() override { return 10; }
    int PointsForBrickDestroyed() override { return 50; }
    int PointsForPaddleHit() override { return 5; }
    std::string GetName() override { return "Classic"; }
};

// hard

class HardScoreStrategy : public IScoreStrategy
{
public:
    int PointsForBrickHit() override { return 20; }
    int PointsForBrickDestroyed() override { return 100; }
    int PointsForPaddleHit() override { return 10; }
    std::string GetName() override { return "Hard"; }
};