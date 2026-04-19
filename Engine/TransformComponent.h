#pragma once



// there's objects have position and own that CMP in COP.
// except x and y, we use one matrix

#include "Component.h"
#include "Matrix2D.h"



namespace XYZengine
{
	class TransformComponent : public Component
	{
	public:
		float x = 0, y = 0;
		float rotation = 0; 
		float scaleX = 1, scaleY = 1;



		// Matrix summary have result of all transformation

		Matrix2D transformMatrix;


		void Update(float deltaTime) override
		{
			// in every frame we summ matrix
			// in next structure: MOVE * ANGLE * SIZE



			float rad = rotation * 3.14159f / 180.0f;

			// Combine all in one matrix
			transformMatrix = Matrix2D::Translation(x, y) * Matrix2D::Rotation(rad);

		}

		
	};
}