#pragma once
#include <string>


// снимок состо€ни€ игры 
// хранит только то, что нужно сохранить

struct GameMemento
{
	int score = 0;
	int record = 0;
	int level = 1;
	float ballSpeedMultiplier = 1.f;
	std::string diffucultyName = "Classic";

// дата сохранени€ (дл€ отображени€ в UI)

std::string saveDate = "";
};

// —оздатель (originator) - умеет создавать и восстановливатьс€ из снимка
// встраиваетс€ в game как набор методов

class GameCaretaker
{
public: 
	// сохранить снимок
	void Save(const GameMemento& memento)
	{
		savedState = memento;
		hasSave = true;
	}


	// восстановить снимок

	GameMemento Load() const
	{
		return savedState;
	}

	bool HasSave() const { return hasSave; }

	// —охранить в файл

    void SaveToFile(const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        file << savedState.score << "\n";
        file << savedState.record << "\n";
        file << savedState.level << "\n";
        file << savedState.ballSpeedMultiplier << "\n";
        file << savedState.diffucultyName << "\n";
        file << savedState.saveDate << "\n";
        file.close();
    }

    // «агрузить из файла
    bool LoadFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        file >> savedState.score;
        file >> savedState.record;
        file >> savedState.level;
        file >> savedState.ballSpeedMultiplier;
        file >> savedState.diffucultyName;
        file >> savedState.saveDate;
        hasSave = true;
        file.close();
        return true;
    }

private:
    GameMemento savedState;
    bool hasSave = false;
};
