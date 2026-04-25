#include "Game.h"
#include "HealthComponent.h"
#include <algorithm>
#include <cassert>
#include <cmath>

// RAII INIT
Game::Game() {
  /*init logger*/
  XYZengine::Logger::Instance().AddSink(
      std::make_unique<XYZengine::ConsoleLogSink>());
  XYZengine::Logger::Instance().AddSink(
      std::make_unique<XYZengine::FileLogSink>("game.log"));
  XYZengine::Logger::Instance().Info("Game logger initialized");

  // resourse load
  if (playinStateMusic.openFromFile(SETTINGS.RESOURCES_PATH +
                                    "PlayingState.ogg")) {
    playinStateMusic.setLoop(true);
  }
  assert(Sound1.loadFromFile(SETTINGS.RESOURCES_PATH + "AppleEat.wav"));
  assert(Sound2.loadFromFile(SETTINGS.RESOURCES_PATH + "Lose.wav"));

  HitSound.setBuffer(Sound1);
  LoseSound.setBuffer(Sound2);

  background.setFillColor(sf::Color::Black);
  background.setSize(sf::Vector2f((float)SETTINGS.SCREEN_WIDTH,
                                  (float)SETTINGS.SCREEN_HEIGHT));
  background.setPosition(0.f, 0.f);

  paddle.Init(*this);
  ball.Init(*this);
  ui.Init(*this);

  LoadRecords();
  SetGameSettings();
  PushGameState(GameState::Menu);
  scoreEventBus.Subscribe(&scoreSystem);

  try {
    resources.LoadSound("bg", "music.ogg");
    XYZengine::Logger::Instance().Info("Background music loaded"); // INFO
  } catch (const std::exception &e) {
    XYZengine::Logger::Instance().Warning(
        std::string("Music loading FAILED: ") + e.what()); // ERROR
  }

  auto *obj = new XYZengine::GameObject();
  auto &audio = obj->AddComponent<XYZengine::AudioComponent>();
  audio.PlayMusic("Resources/music.ogg", true);
  world.AddObject(obj);

  try {
    resources.LoadTexture("player", "Resources/Player.png");
    resources.LoadTexture("enemy", "Resources/apple.png");
    resources.LoadTexture("wall", "Resources/green.png");
  } catch (const std::exception &e) {
    XYZengine::Logger::Instance().Error(e.what()); // ERROR
  }

  /*player*/
  XYZengine::Logger::Instance().Info("Creating player"); // INFO
  player = new XYZengine::GameObject();

  auto &pt = player->AddComponent<XYZengine::TransformComponent>();
  // assert(player->GetComponent<XYZengine::TransformComponent>() != nullptr);
  pt.x = 200.f;
  pt.y = 200.f;

  player->AddComponent<XYZengine::HealthComponent>(100, 50);

  XYZengine::Logger::Instance().Info("Player HealthComponent initialized");
  assert(player->GetComponent<XYZengine::HealthComponent>() != nullptr);

  auto &ps = player->AddComponent<XYZengine::SpriteRenderComponent>();

  assert(player != nullptr); // <- stop

  /*math sprite */
  ps.sprite.setTexture(resources.GetTexture("player"));
  SetSpriteSize(ps.sprite, 64.f, 64.f);
  SetSpriteRelativeOrigin(ps.sprite, 0.5f, 0.5f);

  player->AddComponent<XYZengine::BoxColliderComponent>(64.f, 64.f, true);
  player->AddComponent<XYZengine::InputComponent>();

  world.AddObject(player);

  /* enemy*/
  XYZengine::Logger::Instance().Info("Creating enemy"); // INFO

  enemy = new XYZengine::GameObject();

  auto &et = enemy->AddComponent<XYZengine::TransformComponent>();
  assert(enemy->GetComponent<XYZengine::TransformComponent>() != nullptr);

  et.x = 500.f;
  et.y = 300.f;

  enemy->AddComponent<XYZengine::HealthComponent>(60, 20);
  XYZengine::Logger::Instance().Info("Enemy HealthComponent initialized");
  assert(enemy->GetComponent<XYZengine::HealthComponent>() != nullptr);

  auto &es = enemy->AddComponent<XYZengine::SpriteRenderComponent>();
  es.sprite.setTexture(resources.GetTexture("enemy"));

  assert(enemy != nullptr); // <- stop
  /*math sprite*/
  SetSpriteSize(es.sprite, 48.f, 48.f);
  SetSpriteRelativeOrigin(es.sprite, 0.5f, 0.5f);

  enemy->AddComponent<XYZengine::BoxColliderComponent>(48.f, 48.f, false);

  auto &follow = enemy->AddComponent<XYZengine::EnemyFollowComponent>();
  follow.target = player;

  world.AddObject(enemy);

  CreateLevel(world, resources);

  if (player->GetComponent<XYZengine::TransformComponent>() == nullptr) {
    XYZengine::Logger::Instance().Error(
        "Player TransformComponent is missing after creation");
  }

  if (enemy->GetComponent<XYZengine::TransformComponent>() == nullptr) {
    XYZengine::Logger::Instance().Error(
        "Enemy TransformComponent is missing after creation");
  }

  if (enemy->GetComponent<XYZengine::HealthComponent>() == nullptr) {
    XYZengine::Logger::Instance().Error(
        "Enemy HealthComponent is missing after creation");
  }
}

void Game::InitLevel() {}

void Game::Restart() {}

Game::~Game() {}

void Game::Update(float deltaTime, sf::RenderWindow &window) {

  GameState state = GetCurrentGameState();

  ui.Update(deltaTime, *this, window);

  if (state == GameState::Playing) {
    world.Update(deltaTime);

    Game::PlayerUpdate(deltaTime, window);
    Game::EnemyUpdate(deltaTime, window);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
      SwitchGameState(GameState::Menu);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
      Game::Restart();
    }
  }
}

/*UPDATES*/

inline void Game::PlayerUpdate(float deltaTime, sf::RenderWindow &window) {
  /*PLAYER UPDATE*/
  if (playerAttackCooldown > 0.0f) {
    playerAttackCooldown -= deltaTime;
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) &&
      playerAttackCooldown <= 0.0f) {
    auto *playerTransform =
        player ? player->GetComponent<XYZengine::TransformComponent>()
               : nullptr;
    auto *enemyTransform =
        enemy ? enemy->GetComponent<XYZengine::TransformComponent>() : nullptr;
    auto *enemyHealth =
        enemy ? enemy->GetComponent<XYZengine::HealthComponent>() : nullptr;

    if (!playerTransform || !enemyTransform || !enemyHealth) {
      XYZengine::Logger::Instance().Warning(
          "Player attack skipped: missing transform or health component");
    } else {
      float dx = enemyTransform->x - playerTransform->x;
      float dy = enemyTransform->y - playerTransform->y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance <= 80.0f) {
        XYZengine::Logger::Instance().Info("Player attacked enemy");
        enemyHealth->TakeDamage(20);

        if (enemyHealth->IsDead()) {
          XYZengine::Logger::Instance().Info("Enemy died");
        }
      } else {
        XYZengine::Logger::Instance().Warning(
            "Player attack missed: enemy too far");
      }
    }

    playerAttackCooldown = 0.5f;
  }
}

inline void Game::EnemyUpdate(float deltaTime, sf::RenderWindow &window) {
  if (enemyAttcakCoolDown > 0.0f) {
    enemyAttcakCoolDown -= deltaTime;
  }

  auto *playerTransform =
      player ? player->GetComponent<XYZengine::TransformComponent>() : nullptr;
  auto *enemyTransform =
      enemy ? enemy->GetComponent<XYZengine::TransformComponent>() : nullptr;
  auto *playerHealth =
      player ? player->GetComponent<XYZengine::HealthComponent>() : nullptr;

  if (enemy && player && playerTransform && enemyTransform && playerHealth) {
    float dx = playerTransform->x - enemyTransform->x;
    float dy = playerTransform->y - enemyTransform->y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= 70.0f && enemyAttcakCoolDown <= 0.0f) {
      XYZengine::Logger::Instance().Info("Enemy attacked player");
      playerHealth->TakeDamage(15);
      enemyAttcakCoolDown = 0.5f;
    }

    if (playerHealth->IsDead()) {
      XYZengine::Logger::Instance().Warning("Player died");
      SwitchGameState(GameState::GameOver);
    }
  }

  else {
    XYZengine::Logger::Instance().Warning(
        "EnemyAttack skipped: missing player/enemy transform or player health");
  }
}

void Game::Draw(sf::RenderWindow &window) {
  window.clear();
  window.draw(background);

  GameState state = GetCurrentGameState();

  if (state == GameState::Playing) {
    renderSystem.Render(window, world.GetObjects());
    ui.DrawPlaying(*this, window);
  } else if (state == GameState::GameOver) {
    window.draw(ui.gameOverText);
  }

  else if (state == GameState::Win) {
    ui.DrawWin(window);
  }

  else if (state == GameState::Menu) {
    ui.DrawMenu(window);
  } else if (state == GameState::LeaderBoard) {
    ui.UpdateLeaderboardGameOver(playerRecord, tempPlayerName);
    window.draw(ui.gameOverText);
    window.draw(ui.gameOverScoreText);
    ui.DrawLeaderBoard(window);
  } else if (state == GameState::PauseMenu) {
    ui.UpdateLeaderboardGameOver(playerRecord, tempPlayerName);
    window.draw(ui.gameOverText);
    window.draw(ui.gameOverScoreText);
    ui.DrawPause(window);
  } else if (state == GameState::Options) {
    ui.DrawOptions(window);
  } else if (state == GameState::Diffcult) {
    ui.DrawDiff(window);
  } else if (state == GameState::Cin) {
    ui.DrawCin(*this, window);
  }

  // вывод текста во время игры
  if (state == GameState::Playing && savePopupTimer > 0.f) {
    window.draw(ui.Save);
  }

  if (state == GameState::Playing && loadPopupTimer > 0.f) {
    window.draw(ui.Load);
  }
}

void Game::SetGameSettings() {

  isGameFinished = false;
  timeSinceGameFinish = 0.f;
  scoreEatenApples = 0;

  if (gameMode & (uint32_t)GameSettingBits::EasyMode) {
    ApplyScoreStrategy(std::make_unique<EasyScoreStrategy>());
  }

  else if (gameMode & (uint32_t)GameSettingBits::HardMode) {
    ApplyScoreStrategy(std::make_unique<HardScoreStrategy>());
  }

  else {
    ApplyScoreStrategy(std::make_unique<ClassicScoreStrategy>());
  }
}

// lifo
void Game::PushGameState(GameState state) {
  GameState oldState = GetCurrentGameState();
  SwitchGameStateInternal(oldState, state);
  gameStateStack.push_back(state);
}

void Game::PopGameState() {
  if (gameStateStack.empty())
    return;
  GameState oldState = gameStateStack.back();
  gameStateStack.pop_back();
  GameState newState = GetCurrentGameState();
  SwitchGameStateInternal(oldState, newState);
}

void Game::SwitchGameState(GameState newState) {
  if (!gameStateStack.empty()) {
    GameState oldState = gameStateStack.back();
    gameStateStack.pop_back();
    gameStateStack.push_back(newState);
    SwitchGameStateInternal(oldState, newState);
  } else {
    PushGameState(newState);
  }
}

GameState Game::GetCurrentGameState() const {
  return gameStateStack.empty() ? GameState::None : gameStateStack.back();
}

// level
inline void Game::CreateLevel(XYZengine::GameWorld &world,
                              XYZengine::ResourseSystem &resources) {
  float size = 64.0f; // размер одной плитки (вынести в константы)
  int columns = 15;
  int rows = 10;
  XYZengine::Logger::Instance().Info("Creating level walls"); // INFO

  for (int x = 0; x < columns; x++) {
    for (int y = 0; y < rows; y++) {
      // Ставим стену только если этой край карты
      if (x == 0 || x == columns - 1 || y == 0 || y == rows - 1) {
        auto *wall = new XYZengine::GameObject();

        auto &t = wall->AddComponent<XYZengine::TransformComponent>();
        assert(wall->GetComponent<XYZengine::TransformComponent>() != nullptr);
        t.x = x * size;
        t.y = y * size;

        auto &s = wall->AddComponent<XYZengine::SpriteRenderComponent>();
        s.sprite.setTexture(resources.GetTexture("wall"));

        SetSpriteSize(s.sprite, 64.f, 64.f);
        SetSpriteRelativeOrigin(s.sprite, 0.f, 0.f);

        wall->AddComponent<XYZengine::BoxColliderComponent>(64.0f, 64.0f, true);

        world.AddObject(wall);
      }
    }
  }
}

void Game::SwitchGameStateInternal(GameState oldState, GameState newState) {
  switch (newState) {
  case GameState::Playing:
    ui.StartPlayinState(*this);
    break;
  case GameState::GameOver:
    ui.GameOVERUI(*this);
    break;
  case GameState::Menu:
    ui.StartMenuState();
    break;
  case GameState::LeaderBoard:
    ui.Init(*this);
    break;
  case GameState::PauseMenu:
    ui.PauseState(*this);
    break;
  case GameState::Diffcult:
    ui.StartDiffState(*this);
    break;
  case GameState::Options:
    ui.OptionsState(*this);
    break;
  case GameState::Cin:
    ui.CinState(*this);
    break;

  case GameState::Win:
    ui.StartWinState(*this);
    break;
  default:
    break;
  }
}

// load and save resources
void Game::LoadRecords() {
  std::ifstream file(SAVE_FILE);
  if (file.is_open()) {
    ui.recordsTable.clear();
    std::string name;
    int score;
    while (file >> name >> score) {
      ui.recordsTable[name] = score;
    }
    file.close();
  } else {
    ui.InitializeLeaderBoard();
  }
}

void Game::SaveRecords() {
  std::ofstream file(SAVE_FILE);
  if (file.is_open()) {
    for (const auto &entry : ui.recordsTable) {
      file << entry.first << " " << entry.second << std::endl;
    }
    file.close();
  }
}
