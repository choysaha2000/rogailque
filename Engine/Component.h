#pragma once

// Interface for entity's skills. 

namespace XYZengine
{
	
		class GameObject;

		class Component
		{
		public:
			virtual ~Component() = default;

	
			// & to object-boss so PC could talk to others

			GameObject* owner = nullptr;

			virtual void Init() {}
			virtual void Update (float deltaTime) {}


		};

}