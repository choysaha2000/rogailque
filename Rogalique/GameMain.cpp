// ©2023, XYZ School. All rights reserved.
// Authored by Aleksandr Rybalka (polterageist@gmail.com)

#include "Application.h"
#include "Constants.h"
#include "Game.h"
#include "Player.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

int main() {
  Application::instance().Run();

  return 0;
}
