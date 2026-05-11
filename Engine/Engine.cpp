#include "pch.h"
#include "Engine.h"
#include <iostream>

Engine::Engine() {}

void Engine::Initialize() {
	std::cout << "Engine initilize" << std::endl;
}

void Engine::Run()
{
	std::cout << "Engine have been started!" << std::endl;
}
