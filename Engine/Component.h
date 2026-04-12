#pragma once
// интерефйс для навыко сущностей
namespace XYZengine
{
	
		class GameObject;

		class Component
		{
		public:
			virtual ~Component() = default;

			// Ссылка на объект-владелец, чтобы комп мог обращаться к другим комп

			GameObject* owner = nullptr;

			virtual void Init() {}
			virtual void Update (float deltaTime) {}


		};

}