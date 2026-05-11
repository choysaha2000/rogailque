#pragma once
#include "Constants.h"
#include "Math.h"
#include "Player.h"
#include "SolidBlock.h"
#include "UI.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include "LevelData.h"

/*engine*/
#include "AudioComponent.h"
#include "EnemyFollowComponent.h"
#include "Engine.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "SpriteRenderComponent.h"
#include "TransformComponent.h"

/*Logger*/

#include "Cassert"
#include "ConsoleLogSink.h"
#include "FileLogSink.h"
#include "Logger.h"

const std::string SAVE_FILE = "records.txt";

enum class GameSettingBits {
  EasyMode = 1 << 0,
  MiddleMode = 1 << 1,
  HardMode = 1 << 2,
};

enum class GameState {
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

class Game {
public:
  Game();
  ~Game();

  Game(const Game &) = delete;
  Game &operator=(const Game &) = delete;

  // main cicle
  void Update(float deltaTime, sf::RenderWindow &window);

  void PlayerUpdate(float deltaTime, sf::RenderWindow &window);
  void EnemyUpdate(float deltaTime, sf::RenderWindow &window);

  void Draw(sf::RenderWindow &window);

  // state machines
  void PushGameState(GameState state);
  void PopGameState();
  void SwitchGameState(GameState newState);
  GameState GetCurrentGameState() const;

  // main gets and sets
  Paddle &GetPaddle() { return paddle; }
  Ball &GetBall() { return ball; }
  UI &GetUI() { return ui; }

  void InitPaddle() { paddle.Init(*this); }
  void InitBall() { ball.Init(*this); }

  // main gets and sets
  bool GetIsInputActive() const { return isInputActive; }
  void SetIsInputAcitve(bool val) { isInputActive = val; }
  std::string &GetTempPlayerName() { return tempPlayerName; }
  void SetTempPlayerName(const std::string &name) { tempPlayerName = name; }

  // records get and set
  void AddScore(int points) {
    scoreEatenApples += points;
    if (scoreEatenApples > playerRecord)
      playerRecord = scoreEatenApples;
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
  void PlayHitSound() {
    if (isSoundOn)
      HitSound.play();
  }
  void PlayLoseSound() {
    if (isSoundOn)
      LoseSound.play();
  }
  void PlayMusic() {
    if (isMusicOn && playinStateMusic.getStatus() != sf::Music::Playing)
      playinStateMusic.play();
  }
  void StopMusic() { playinStateMusic.stop(); }

  void SetBackgroundColor(sf::Color color) { background.setFillColor(color); }


  uint32_t GetgameMode() const { return gameMode; }
  void SetgameMode(uint32_t gameModes) { gameMode = gameModes; }

  void CreateLevel(const LevelData &level);


  void InitLevels();
  void LoadLevel(int index);
  void LoadNextLevel();
  void CheckLevelCompletion();

  void CreatePlayer(const LevelData &level);
  void CreateEnemies(const LevelData &level);
  void ResetMenu();



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
  sf::Sprite levelBackgroundSprite;
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
  
  float bonusTimer = 0.f;

  int currentLevel = 1;

  float savePopupTimer = 0.f;
  float loadPopupTimer = 0.f;

  sf::RenderWindow window;

  std::vector <LevelData> levels;
  int currentLevelIndex = 0;
  bool levelCompleted = false;
  int currentEnemyDamage = 15;

  /*engine shit*/

  XYZengine::GameWorld world;
  XYZengine::ResourseSystem resources;
  XYZengine::RenderSystem renderSystem;

  XYZengine::GameObject *player = nullptr;
  
  std::vector<XYZengine::GameObject *> enemies;

  int aliveEnemies = 0;

  float playerAttackCooldown = 0.0f;
  float enemyAttcakCoolDown = 0.0f;

  float EnemyPOS = 0.0f;
  
};
