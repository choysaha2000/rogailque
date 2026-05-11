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

  HitSound.setBuffer(Sound1);
  LoseSound.setBuffer(Sound2);

  background.setFillColor(sf::Color::Black);
  background.setSize(sf::Vector2f((float)SETTINGS.SCREEN_WIDTH,
                                  (float)SETTINGS.SCREEN_HEIGHT));
  background.setPosition(0.f, 0.f);

  ui.Init(*this);
  PushGameState(GameState::Menu);
  
  //world.AddObject(obj);

  if (playinStateMusic.openFromFile("Resources/music.ogg")) {
    playinStateMusic.setLoop(true);
    XYZengine::Logger::Instance().Info("Background music loaded");
  } else {
    XYZengine::Logger::Instance().Warning("Background music loading failed");
  }


  try {
    resources.LoadTexture("player", "Resources/Cheetah/Default.png");
    resources.LoadTexture("hyena", "Resources/Hyena/Default.png"); // ????
    resources.LoadTexture("wall", "Resources/green.png");
    resources.LoadTexture("leopard", "Resources/Leopard/Default.png");
    resources.LoadTexture("lion", "Resources/Lion/Default.png");
    resources.LoadTexture("map_savanna", "Resources/maps/desert.png");
    resources.LoadTexture("map_jungle", "Resources/maps/jungle.png");
    resources.LoadTexture("map_canyon", "Resources/maps/final.png");
    


  } catch (const std::exception &e) {
    XYZengine::Logger::Instance().Error(e.what()); // ERROR
  }

  /*player*/
  InitLevels();
  LoadLevel(0);
  /* enemy*/
  
  if (player->GetComponent<XYZengine::TransformComponent>() == nullptr) {
    XYZengine::Logger::Instance().Error(
        "Player TransformComponent is missing after creation");
  }

 /* if (enemies->GetComponent<XYZengine::TransformComponent>() == nullptr) {
    XYZengine::Logger::Instance().Error(
        "Enemy TransformComponent is missing after creation");
  }

  if (enemy->GetComponent<XYZengine::HealthComponent>() == nullptr) {
    XYZengine::Logger::Instance().Error(
        "Enemy HealthComponent is missing after creation");
  }

  */

}

void Game::CreatePlayer(const LevelData& level)
{
  XYZengine::Logger::Instance().Info("Creating player"); // INFO
  player = new XYZengine::GameObject();

  auto &pt = player->AddComponent<XYZengine::TransformComponent>();
  pt.x = level.playerSpawnX;
  pt.y = level.playerSpawnY;
  assert(player->GetComponent<XYZengine::TransformComponent>() != nullptr);


  player->AddComponent<XYZengine::HealthComponent>(150, 50);
  XYZengine::Logger::Instance().Info("Player HealthComponent initialized");
  assert(player->GetComponent<XYZengine::HealthComponent>() != nullptr);

  auto &ps = player->AddComponent<XYZengine::SpriteRenderComponent>();
  ps.sprite.setTexture(resources.GetTexture("player"));
  SetSpriteSize(ps.sprite, 64.0f, 64.0f);

  player->AddComponent < XYZengine::BoxColliderComponent>(64.f, 64.f, true);
  player->AddComponent<XYZengine::InputComponent>();

  world.AddObject(player);

  assert(player != nullptr); // <- stop
}
void Game::CreateEnemies(const LevelData& level)
{
  XYZengine::Logger::Instance().Info("Creating enemies: " +
                                     level.enemyTextureKey);

  enemies.clear();
  aliveEnemies = 0;


  for (int i = 0; i < level.enemyCount; i++)
  {
    auto *enemy = new XYZengine::GameObject();

    auto &et = enemy->AddComponent<XYZengine::TransformComponent>();

    float offsetX = static_cast<float>((i % 5) * 64);
    float offsetY = static_cast<float>((i / 5) * 64);

    et.x = level.enemySpawnX + offsetX;
    et.y = level.enemySpawnY + offsetY;


    enemy->AddComponent<XYZengine::HealthComponent>(level.enemyHealth,
                                                    level.enemyArmor);

    auto &es = enemy->AddComponent<XYZengine::SpriteRenderComponent>();
    es.sprite.setTexture(resources.GetTexture(level.enemyTextureKey));
    SetSpriteSize(es.sprite, 48.f, 48.f);
    SetSpriteRelativeOrigin(es.sprite, 0.5f, 0.5f);

    enemy->AddComponent<XYZengine::BoxColliderComponent>(

        48.f, 48.f, false);

    auto &follow = enemy->AddComponent<XYZengine::EnemyFollowComponent>();
    follow.target = player;
    follow.speed = level.enemySpeed;

    world.AddObject(enemy);
    enemies.push_back(enemy);
    aliveEnemies++;
  }

  currentEnemyDamage = level.enemyDamage;

  XYZengine::Logger::Instance().Info("Enemies created: " +
                                     std::to_string(aliveEnemies));
 ;

}

void Game::ResetMenu() {

      XYZengine::Logger::Instance().Info("Resetting game to menu");

  SetTimeSinceGameFinish(0.0f);
  SetBackgroundColor(sf::Color::Black);

  currentLevelIndex = 0;
  levelCompleted = false;

  playerAttackCooldown = 0.0f;
  enemyAttcakCoolDown = 0.0f;

  InitLevels();
  LoadLevel(0);

  SwitchGameState(GameState::Menu);
}

Game::~Game() {}

void Game::Update(float deltaTime, sf::RenderWindow &window) {

  GameState state = GetCurrentGameState();

  ui.Update(deltaTime, *this, window);

  if (state == GameState::Playing) {

    world.Update(deltaTime);

    Game::PlayerUpdate(deltaTime, window);
    Game::EnemyUpdate(deltaTime, window);
    CheckLevelCompletion();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
      SwitchGameState(GameState::Menu);
    }

  }
   else if (state == GameState::GameOver)
  {
    if (GetGameFinishTime() <= SETTINGS.PAUSE_LENGTH) {
      AddGameFinishTime(deltaTime);
      SetBackgroundColor(sf::Color::Red);
    } else {
      ResetMenu();
    }
  }

   else if (state == GameState::Win)
  {
    if (GetGameFinishTime() <= SETTINGS.PAUSE_LENGTH) {
      AddGameFinishTime(deltaTime);
      SetBackgroundColor(sf::Color::Green);
    } else {
      ResetMenu();
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

    if (!playerTransform) {
      XYZengine::Logger::Instance().Warning(
          "Player attack skipped: player transform missing");
      playerAttackCooldown = 0.5f;
      return;
    }

    XYZengine::GameObject *targetEnemy = nullptr;
    XYZengine::HealthComponent *targetHealth = nullptr;
    float bestDistance = 999999.0f;

    for (auto *enemy : enemies) {
      if (!enemy) {
        continue;
      }

      auto *enemyTransform =
          enemy->GetComponent<XYZengine::TransformComponent>();
      auto *enemyHealth = enemy->GetComponent<XYZengine::HealthComponent>();

      if (!enemyTransform || !enemyHealth || enemyHealth->IsDead()) {
        continue;
      }

      float dx = enemyTransform->x - playerTransform->x;
      float dy = enemyTransform->y - playerTransform->y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance < bestDistance) {
        bestDistance = distance;
        targetEnemy = enemy;
        targetHealth = enemyHealth;
      }
    }

    if (!targetEnemy || !targetHealth) {
      XYZengine::Logger::Instance().Warning(
          "Player attack skipped: no living enemy");
    } else if (bestDistance <= 80.0f) {
      XYZengine::Logger::Instance().Info("Player attacked enemy");

      bool wasAlive = !targetHealth->IsDead();
      targetHealth->TakeDamage(20);

      if (wasAlive && targetHealth->IsDead()) {
        aliveEnemies--;

        if (auto* render = targetEnemy->GetComponent<XYZengine::SpriteRenderComponent>())
        {
          render->sprite.setColor(sf::Color(255, 255, 255, 0));
        }


        XYZengine::Logger::Instance().Info("Enemy died. Alive enemies: " +
                                           std::to_string(aliveEnemies));
      }
    } else {
      XYZengine::Logger::Instance().Warning(
          "Player attack missed: enemy too far");
    
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
  auto *playerHealth =
      player ? player->GetComponent<XYZengine::HealthComponent>() : nullptr;
    
  if (!player || !playerTransform || !playerHealth)
  {
    XYZengine::Logger::Instance().Warning(
        "EnemyAttack skipped: missing player transform or player health");
    return;
  }

  if (enemyAttcakCoolDown <= 0.0f)
  {
      for (auto* enemy : enemies)
      {
          if (!enemy)
          {
          continue;
      }
        
        auto *enemyTransform =
          enemy->GetComponent<XYZengine::TransformComponent>();
      auto *enemyHealth = enemy->GetComponent<XYZengine::HealthComponent>();


      if (!enemyTransform || !enemyHealth || enemyHealth->IsDead())
      {
        continue;
      }

      float dx = playerTransform->x - enemyTransform->x;
      float dy = playerTransform->y - enemyTransform->y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance <= 70.0f)
      {
        XYZengine::Logger::Instance().Info("Enemy attacked player");
        playerHealth->TakeDamage(currentEnemyDamage);
        enemyAttcakCoolDown = 0.5f;
        break;
      }

    }
  }



  if (playerHealth->IsDead())
  {
    XYZengine::Logger::Instance().Warning("Player died");
    SwitchGameState(GameState::GameOver);
  }

}

void Game::Draw(sf::RenderWindow &window) {
  window.clear();

  GameState state = GetCurrentGameState();

  if (state == GameState::Playing) {
    window.draw(levelBackgroundSprite);
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
inline void Game::CreateLevel(const LevelData &level) {

    XYZengine::Logger::Instance().Info("Creating level walls for: " + level.name);

    for (int x = 0; x < level.colums; x++) {
    
        for (int y = 0; y < level.rows; y++) {
        if (x == 0 || x == level.colums - 1 || y == 0 || y == level.rows - 1) {
            auto *wall = new XYZengine::GameObject();

            auto &t = wall->AddComponent<XYZengine::TransformComponent>();
            t.x = x * level.tileSize;
            t.y = y * level.tileSize;


            auto &s = wall->AddComponent<XYZengine::SpriteRenderComponent>();
            s.sprite.setTexture(resources.GetTexture("wall"));
            SetSpriteSize(s.sprite, 64.f, 64.f);
            SetSpriteRelativeOrigin(s.sprite, 0.f, 0.f);

            wall->AddComponent <
                XYZengine::BoxColliderComponent>(64.0f, 64.0f, true);
            world.AddObject(wall);
        }
        }
    }

  
}

void Game::SwitchGameStateInternal(GameState oldState, GameState newState) {
  switch (newState) {
  case GameState::Playing:
    ui.StartPlayinState(*this);
    PlayMusic();
    break;
  case GameState::GameOver:
    StopMusic();
    ui.GameOVERUI(*this);
    break;
  case GameState::Menu:
    StopMusic();
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

void Game::InitLevels() { 
    levels.clear();

  
      levels.push_back({"Savanna", 15, 10, 64.0f, 128.0f, 128.0f, 640.0f,
                        384.0f, 60, 20, 10, 60.0f, 3, "map_savanna", "hyena"});

        
       levels.push_back({"Jungle", 15, 10, 64.0f, 128.0f, 512.0f, 704.0f, 128.0f,
                        70, 30, 15, 65.0f, 2, "map_jungle", "leopard"});

       levels.push_back({"Rocky Canyon", 15, 10, 64.0f, 128.0f, 128.0f, 768.0f,
                         512.0f, 90, 50, 20, 90.0f, 1, "map_canyon", "lion"});

       XYZengine::Logger::Instance().Info("Level init");

}

void Game::LoadLevel(int index) {
  if (index < 0 || index >= static_cast<int>(levels.size())) {
    XYZengine::Logger::Instance().Error(
        "LoadLevel failed: invalid level index");
    return;
  }

  world.Clear();
  player = nullptr;
  enemies.clear();

  aliveEnemies = 0;

  currentLevelIndex = index;
  levelCompleted = false;
  playerAttackCooldown = 0.0f;
  enemyAttcakCoolDown = 0.0f;

  const LevelData &level = levels[currentLevelIndex];

  levelBackgroundSprite.setTexture(resources.GetTexture(level.mapTextureKey));
  SetSpriteSize(levelBackgroundSprite, (float)SETTINGS.SCREEN_WIDTH,
                (float)SETTINGS.SCREEN_HEIGHT);
  SetSpriteRelativeOrigin(levelBackgroundSprite, 0.f, 0.f);
  levelBackgroundSprite.setPosition(0.f, 0.f);


  XYZengine::Logger::Instance().Info("Loading level: " + level.name);

  CreateLevel(level);
  CreatePlayer(level);
  CreateEnemies(level);

  XYZengine::Logger::Instance().Info("Level loaded: " + level.name);

}

void Game::LoadNextLevel()
{ int nextLevel = currentLevelIndex + 1;
  
if (nextLevel >= static_cast<int>(levels.size()))
{
    XYZengine::Logger::Instance().Info("All level completed");
  SwitchGameState(GameState::Win);
    return;
  }

XYZengine::Logger::Instance().Info("Loading next level");

  LoadLevel(nextLevel);
}

void Game::CheckLevelCompletion() {
  if (levelCompleted)    {
      return;
    }

    if (aliveEnemies <= 0) {

      levelCompleted = true;
      XYZengine::Logger::Instance().Info(
          "All enemies defeated, level completed");
      LoadNextLevel();
      return;
    }
  }
