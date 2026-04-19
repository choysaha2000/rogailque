#pragma once
#include "Constants.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h" 
#include "UI.h"
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream> 
#include <vector> 
#include "Math.h"
#include "Block.h"
#include <memory>
#include "SolidBlock.h"

#include "ScoreObserver.h"
#include "GameMemento.h"
#include "Bonus.h"
#include "BonusStrategy.h"
#include "ScoreStrategy.h"
#include <ctime>

/*engine*/
#include "Engine.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "ResourceSystem.h"
#include "SpriteRenderComponent.h"
#include "AudioComponent.h"
#include "InputComponent.h"
#include "EnemyFollowComponent.h"
#include "RenderSystem.h"
#include "HealthComponent.h"

/*Logger*/

#include "Logger.h"
#include "ConsoleLogSink.h"
#include "FileLogSink.h"
#include "Cassert"

const std::string SAVE_FILE = "records.txt";

enum class GameSettingBits
{
    EasyMode = 1 << 0,
    MiddleMode = 1 << 1,
    HardMode = 1 << 2,
};

enum class GameState
{
    None = 0,
    Menu,
    Playing,
    GameOver,
    LeaderBoard,
    PauseMenu,
    Options,
    Diffcult,
    Cin,
    Win
};

class Game
{
public:
    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // main cicle
    void Update(float deltaTime, sf::RenderWindow& window);

    void PlayerUpdate(float deltaTime, sf::RenderWindow& window);
    void EnemyUpdate(float deltaTime, sf::RenderWindow& window);

    void Draw(sf::RenderWindow& window);
    void Restart();
    void SetGameSettings();
    void InitLevel();

    // state machines
    void PushGameState(GameState state);
    void PopGameState();
    void SwitchGameState(GameState newState);
    GameState GetCurrentGameState() const;


   //main gets and sets
    Paddle& GetPaddle() { return paddle; }
    Ball& GetBall() { return ball; }
    UI& GetUI() { return ui; }

    void InitPaddle() { paddle.Init(*this); }
    void InitBall() { ball.Init(*this); }

    //main gets and sets
    bool GetIsInputActive() const { return isInputActive; }
    void SetIsInputAcitve(bool val) { isInputActive = val; }
    std::string& GetTempPlayerName() { return tempPlayerName; }
    void SetTempPlayerName(const std::string& name) { tempPlayerName = name; }

    // records get and set
    void AddScore(int points) {
        scoreEatenApples += points;
        if (scoreEatenApples > playerRecord) playerRecord = scoreEatenApples;
    }

    // game over get and set
    bool IsGameFinished() const { return isGameFinished; }
    void SetGameFinished(bool value) { isGameFinished = value; }

    float GetGameFinishTime() const { return timeSinceGameFinish; }
    void AddGameFinishTime(float delta) { timeSinceGameFinish += delta; }
    void SetTimeSinceGameFinish(float val) { timeSinceGameFinish = val; } 

    // settings get and set
    bool IsSoundOn() const { return isSoundOn; }
    bool IsMusicOn() const { return isMusicOn; }
    bool IsHard() const { return isHard; }

    // audio get and set
    void PlayHitSound() { if (isSoundOn) HitSound.play(); }
    void PlayLoseSound() { if (isSoundOn) LoseSound.play(); }
    void PlayMusic() { if (isMusicOn && playinStateMusic.getStatus() != sf::Music::Playing) playinStateMusic.play(); }
    void StopMusic() { playinStateMusic.stop(); }

    
    void SetBackgroundColor(sf::Color color) { background.setFillColor(color); }


    //block
    std::vector<std::unique_ptr<Block>>& GetBlocks() { return blocks; }

    

    // observer

    void NotifyScore(ScoreEvent event)
    {
        int points = scoreSystem.CalcPoints(event);
        scoreEventBus.Publish(event, points); 
    }

    int GetScore() const { return scoreSystem.GetScore(); }
    int GetRecord() const { return scoreSystem.GetRecord(); }
    
    void ResetScore() { scoreSystem.Reset(); }

    // memento
    GameMemento CreateMemento() const
    {
        GameMemento m;
        m.score = scoreSystem.GetScore();
        m.record = scoreSystem.GetRecord();
        m.level = currentLevel;
        m.diffucultyName = scoreSystem.GetStrategyName();
        // Дата
        time_t t = time(nullptr);
        char buf[64];
        struct tm timeinfo;
        localtime_s(&timeinfo, &t);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &timeinfo);
        m.saveDate = std::string(buf);
        return m;
    }

    void RestoreFromMemento( const GameMemento & m )
    {
       // восстановливаем точные очки и уровень из файла
        scoreSystem.SetScore(m.score);
        scoreSystem.SetRecord(m.record);
        currentLevel = m.level;

        // сбрасываем объекты на поле (рестарт)
        InitLevel();
        paddle.Init(*this);
        ball.Init(*this);

        // очищаем бонусы

        bonuses.clear();

        if (activeBonusEffect != nullptr)
        {
            activeBonusEffect->Remove(ball, paddle); 
            activeBonusEffect.reset();
        }

        // обновляем текст очков в интерфейсе
        ui.scoreText.setString("Score: " + std::to_string(GetScore()));
    }

    void QuickSave()
    {
        auto memento = CreateMemento();
        caretaker.Save(memento);
        caretaker.SaveToFile("quicksave.dat");
    }

    void QuickLoad()
    {
        if (caretaker.LoadFromFile("quicksave.dat"))
            RestoreFromMemento(caretaker.Load());
    }

    bool HasQuickSave() const { return caretaker.HasSave(); }


    // strategy бонусы

    void ActivateBonus(std::unique_ptr<IBonusEffect> effect, float duration)
    {
        if (activeBonusEffect != nullptr)
        {
            activeBonusEffect->Remove(ball, paddle);
         }

        activeBonusEffect = std::move(effect);
        bonusTimer = duration;
    }

    // меняем стратегию очков при смене сложности

    void ApplyScoreStrategy(std::unique_ptr<IScoreStrategy> s)
    {
        scoreSystem.SetStrategy(std::move(s));
    }

    uint32_t GetgameMode() const { return gameMode; }
    void SetgameMode(uint32_t gameModes) { gameMode = gameModes; }


    void CreateLevel(XYZengine::GameWorld& world, XYZengine::ResourseSystem& resources);
private:
    // local logic
    void SwitchGameStateInternal(GameState oldState, GameState newState);

    void LoadRecords();
    void SaveRecords();

private:
    
    Paddle paddle;
    Ball ball;
    UI ui;

  
    math Math;
    sf::RectangleShape background;
    sf::RectangleShape wallTop;



    uint32_t gameMode = 0;
    bool isGameFinished = false;
    float timeSinceGameFinish{};
    std::vector<GameState> gameStateStack;

    int scoreEatenApples = 0; 
    int playerRecord = 0;    

    
    sf::Music playinStateMusic;
    sf::SoundBuffer Sound1, Sound2;
    sf::Sound HitSound, LoseSound;


    bool isMusicOn = true;
    bool isSoundOn = true;
    bool isGamePause = false;
    bool isInputActive = false;

  
    bool isEasy = false;
    bool isMiddle = false;
    bool isHard = false;
    bool isVeryHard = false;
    bool isHardcore = false;

    std::string tempPlayerName = "";

    friend class Application;


    std::vector<std::unique_ptr<Block>> blocks;

    ScoreSystem scoreSystem{ std::make_unique<ClassicScoreStrategy>() };
    ScoreEventBus scoreEventBus;
    GameCaretaker caretaker;

    std::vector<Bonus> bonuses;
    std::unique_ptr<IBonusEffect> activeBonusEffect = nullptr;
    float bonusTimer = 0.f;

    int currentLevel = 1;

    float savePopupTimer = 0.f;
    float loadPopupTimer = 0.f;

    sf::RenderWindow window;

    /*engine shit*/

    XYZengine::GameWorld world;
    XYZengine::ResourseSystem resources;
    XYZengine::RenderSystem renderSystem;

    XYZengine::GameObject* player = nullptr;
    XYZengine::GameObject* enemy = nullptr;

    float playerAttackCooldown = 0.0f;
    float enemyAttcakCoolDown = 0.0f;

    float EnemyPOS = 0.0f;
};
