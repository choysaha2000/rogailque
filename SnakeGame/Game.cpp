#include "Game.h"
#include <cassert>
#include <algorithm> 



// RAII INIT
Game::Game()
{
  // resourse load
    if (playinStateMusic.openFromFile(SETTINGS.RESOURCES_PATH + "PlayingState.ogg")) {
        playinStateMusic.setLoop(true);
    }
    assert(Sound1.loadFromFile(SETTINGS.RESOURCES_PATH + "AppleEat.wav")); 
    assert(Sound2.loadFromFile(SETTINGS.RESOURCES_PATH + "Lose.wav"));     

    HitSound.setBuffer(Sound1);
    LoseSound.setBuffer(Sound2);

    background.setFillColor(sf::Color::Black); 
    background.setSize(sf::Vector2f((float)SETTINGS.SCREEN_WIDTH, (float)SETTINGS.SCREEN_HEIGHT));
    background.setPosition(0.f, 0.f);

    paddle.Init(*this);
    ball.Init(*this);
    ui.Init(*this);


    LoadRecords(); 
    SetGameSettings();
    PushGameState(GameState::Menu);
    scoreEventBus.Subscribe(&scoreSystem);

    resources.LoadSound("bg", "music.ogg");
    auto* obj = new XYZengine::GameObject();
    auto& audio = obj->AddComponent<XYZengine::AudioComponent>();
    audio.PlayMusic("Resources/music.ogg", true);
    world.AddObject(obj);

    resources.LoadTexture("player", "Resources/Player.png");
    resources.LoadTexture("enemy", "Resources/apple.png");
    resources.LoadTexture("wall", "Resources/green.png");




    /*player*/
    player = new XYZengine::GameObject();

    auto& pt = player->AddComponent<XYZengine::TransformComponent>();
    pt.x = 200.f;
    pt.y = 200.f;

    auto& ps = player->AddComponent<XYZengine::SpriteRenderComponent>();

    /*math sprite */
    ps.sprite.setTexture(resources.GetTexture("player"));
    SetSpriteSize(ps.sprite, 64.f, 64.f);
    SetSpriteRelativeOrigin(ps.sprite, 0.5f, 0.5f);

    player->AddComponent<XYZengine::BoxColliderComponent>(64.f,64.f);
    player->AddComponent<XYZengine::InputComponent>();

    world.AddObject(player);


    /* enemy*/
    auto* enemy = new XYZengine::GameObject();
    auto& et = enemy->AddComponent<XYZengine::TransformComponent>();

    et.x = 500.f;
    et.y = 300.f;

    auto& es = enemy->AddComponent<XYZengine::SpriteRenderComponent>();
    es.sprite.setTexture(resources.GetTexture("enemy"));

    /*math sprite*/
    SetSpriteSize(es.sprite, 48.f, 48.f);
    SetSpriteRelativeOrigin(es.sprite, 0.5f, 0.5f);

    enemy->AddComponent<XYZengine::BoxColliderComponent>(48.f, 48.f);

    auto& follow = enemy->AddComponent<XYZengine::EnemyFollowComponent>();
    follow.target = player;

    world.AddObject(enemy);

    CreateLevel(world, resources);

}

void Game::InitLevel()
{
    blocks.clear();

    for (int i = 0; i < SETTINGS.rows; ++i) {
        for (int j = 0; j < SETTINGS.cols; ++j) {

            if (i == 0) {
                auto brick = std::make_unique<SolidBrick>();
                brick->Spawn({ SETTINGS.startX + j * SETTINGS.gapX, SETTINGS.startY + i * SETTINGS.gapY }, sf::Color::Red);
                blocks.push_back(std::move(brick));
            }
            else {
                auto brick = std::make_unique<Block>();
                sf::Color color = (i % 2 == 0) ? sf::Color::Green : sf::Color::Blue;
                brick->Spawn({ SETTINGS.startX + j * SETTINGS.gapX, SETTINGS.startY + i * SETTINGS.gapY }, color);
                blocks.push_back(std::move(brick));
            }
        }
    }
}

void Game::Restart()
{
    paddle.Init(*this);
    ball.Init(*this);
    SetGameSettings();
    scoreEatenApples = 0; 
    PushGameState(GameState::Menu);
}

Game::~Game()
{
}


// UPDATE WINDOW ARGUMENT HERE CUZ THERE'S MOUSE INPUT 
void Game::Update(float deltaTime, sf::RenderWindow& window)
{
    GameState state = GetCurrentGameState();

    ui.Update(deltaTime, *this, window);

    if (state == GameState::Playing)
    {
       
        world.Update(deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            SwitchGameState(GameState::Menu);
        }
    }

   



}


void Game::Draw(sf::RenderWindow& window)
{
    window.clear();
    window.draw(background);
   
    GameState state = GetCurrentGameState();

    if (state == GameState::Playing)
    {
        renderSystem.Render(window, world.GetObjects());
        ui.DrawPlaying(*this, window);
    }
    else if (state == GameState::GameOver)
    {
        window.draw(ui.gameOverText);
    }

    else if (state == GameState::Win) 
    {
        ui.DrawWin(window); 
    }

    else if (state == GameState::Menu)
    {
        ui.DrawMenu(window);
    }
    else if (state == GameState::LeaderBoard)
    {
        ui.UpdateLeaderboardGameOver(playerRecord, tempPlayerName);
        window.draw(ui.gameOverText);
        window.draw(ui.gameOverScoreText);
        ui.DrawLeaderBoard(window);
    }
    else if (state == GameState::PauseMenu)
    {
        ui.UpdateLeaderboardGameOver(playerRecord, tempPlayerName);
        window.draw(ui.gameOverText);
        window.draw(ui.gameOverScoreText);
        ui.DrawPause(window);
    }
    else if (state == GameState::Options)
    {
        ui.DrawOptions(window);
    }
    else if (state == GameState::Diffcult)
    {
        ui.DrawDiff(window);
    }
    else if (state == GameState::Cin)
    {
        ui.DrawCin(*this, window);
    }

    // вывод текста во время игры 
    if (state == GameState::Playing && savePopupTimer > 0.f)
    {
        window.draw(ui.Save);
    }

    if (state == GameState::Playing && loadPopupTimer > 0.f)
    {
        window.draw(ui.Load);
    }
}

void Game::SetGameSettings()
{
    
    isGameFinished = false;
    timeSinceGameFinish = 0.f;
    scoreEatenApples = 0;

    if (gameMode & (uint32_t)GameSettingBits::EasyMode)
    {
        ApplyScoreStrategy(std::make_unique<EasyScoreStrategy>());
    }

    else if (gameMode & (uint32_t)GameSettingBits::HardMode)
    {
        ApplyScoreStrategy(std::make_unique<HardScoreStrategy>());
    }

    else
    {
        ApplyScoreStrategy(std::make_unique<ClassicScoreStrategy>());
    }

 


}


// lifo
void Game::PushGameState(GameState state)
{
    GameState oldState = GetCurrentGameState();
    SwitchGameStateInternal(oldState, state);
    gameStateStack.push_back(state);
}

void Game::PopGameState()
{
    if (gameStateStack.empty()) return;
    GameState oldState = gameStateStack.back();
    gameStateStack.pop_back();
    GameState newState = GetCurrentGameState();
    SwitchGameStateInternal(oldState, newState);
}

void Game::SwitchGameState(GameState newState)
{
    if (!gameStateStack.empty()) {
        GameState oldState = gameStateStack.back();
        gameStateStack.pop_back();
        gameStateStack.push_back(newState);
        SwitchGameStateInternal(oldState, newState);
    } else {
        PushGameState(newState);
    }
}

GameState Game::GetCurrentGameState() const
{
    return gameStateStack.empty() ? GameState::None : gameStateStack.back();
}


// level 
inline void Game::CreateLevel(XYZengine::GameWorld& world, XYZengine::ResourseSystem& resources)
{
    float size = 64.0f; // размер одной плитки (вынести в константы)
    int columns = 15;
    int rows = 10;

    for (int x = 0; x < columns; x++)
    {
        for (int y = 0; y < rows; y++)
        {
            // Ставим стену только если этой край карты
            if (x == 0 || x == columns - 1 || y == 0 || y == rows - 1)
            {
                auto* wall = new XYZengine::GameObject();

                auto& t = wall->AddComponent<XYZengine::TransformComponent>();
                t.x = x * size;
                t.y = y * size;

                auto& s = wall->AddComponent<XYZengine::SpriteRenderComponent>();
                s.sprite.setTexture(resources.GetTexture("wall"));

                SetSpriteSize(s.sprite, 64.f, 64.f);
                SetSpriteRelativeOrigin(s.sprite, 0.f, 0.f);

                wall->AddComponent<XYZengine::BoxColliderComponent>();

                world.AddObject(wall);

            }
        }
    }

}

void Game::SwitchGameStateInternal(GameState oldState, GameState newState)
{
    switch (newState)
    {
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
void Game::LoadRecords()
{
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

void Game::SaveRecords()
{
    std::ofstream file(SAVE_FILE);
    if (file.is_open()) {
        for (const auto& entry : ui.recordsTable) {
            file << entry.first << " " << entry.second << std::endl;
        }
        file.close();
    }
}



