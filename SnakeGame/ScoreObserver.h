#pragma once
#include <vector>
#include <memory>
#include "ScoreStrategy.h"

enum class ScoreEvent
{
	BrickHit, // м€ч попал в кирпич, но не уничтожил
	BrickDestroyed, // кирпич уничтожен
	PaddleHit, // м€ч отбит платформой
};


// интерфейс обсервера

class IScoreObserver
{
public: 
	virtual ~IScoreObserver() = default;
	virtual void OnScoreEvent(ScoreEvent event, int points) = 0;
};

// —истема очков использует наблюдатель + стратегию

class ScoreSystem : public IScoreObserver
{
public: 
	explicit ScoreSystem(std::unique_ptr<IScoreStrategy> strategy)
		: strategy(std::move(strategy)), score(0), record(0) {
	}

	void OnScoreEvent(ScoreEvent event, int points) override
	{
		score += points;
		if (score > record) record = score;
	}


	// начисл€ть очки по стратегии

	int CalcPoints(ScoreEvent event) const
	{
		switch (event)
		{
		case ScoreEvent::BrickHit:       return strategy->PointsForBrickHit();
		case ScoreEvent::BrickDestroyed: return strategy->PointsForBrickDestroyed();
		case ScoreEvent::PaddleHit:      return strategy->PointsForPaddleHit();
		default: return 0;

		}
	}
	void Reset() { score = 0; }
	void SetScore(int s) { score = s; }
	void SetRecord(int r) { record = r; }
	int GetScore() const { return score; }
	int GetRecord() const { return record; }
	std::string GetStrategyName() const {
		return strategy->GetName();
	}

	// мен€ем стратегию в процессе (при смене сложности)

	void SetStrategy(std::unique_ptr<IScoreStrategy> newStrategy)
	{
		strategy = std::move(newStrategy);
	}


private:
	std::unique_ptr<IScoreStrategy> strategy;
	int score;
	int record;
};

// издатель событий - рассылает событи€ всем наблюдател€м

class ScoreEventBus
{
public: 

	void Subscribe(IScoreObserver* observer)
	{
		observers.push_back(observer);
	}

	void Publish(ScoreEvent event, int points)
	{
		for (auto* obs : observers)
			obs->OnScoreEvent(event, points);

	}

private: 
	std::vector <IScoreObserver*> observers;
};