// ©2023, XYZ School. All rights reserved.
// Authored by Aleksandr Rybalka (polterageist@gmail.com)

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Engine.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "TransformComponent.h"

const std::string RESOURCES_PATH = "Resources/";

int main()
{

	Engine engine;
	engine.Initialize();
	engine.Run();

	XYZengine::GameWorld world;

	auto* obj = new XYZengine::GameObject();
	world.AddObject(obj);

	obj->AddComponent<XYZengine::TransformComponent>();

	world.Update(0.016f);

	return 0;
}
