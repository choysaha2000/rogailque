#pragma once

struct LevelData
{
  std::string name;

  int colums = 15;
  int rows = 10;
  float tileSize = 64.0f;

  float playerSpawnX = 128.0f;
  float playerSpawnY = 128.0f;

  float enemySpawnX = 320.0f;
  float enemySpawnY = 256.0f;

  int enemyHealth = 60;
  int enemyArmor = 20;
  int enemyDamage = 10;
  float enemySpeed = 100.0f;
  int enemyCount = 5;

  std::string mapTextureKey;
  std::string enemyTextureKey;


};