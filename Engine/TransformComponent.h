#pragma once
//В КОП каждый объект должен иметь позицию и владеть этим компонентом
// Вместо переменных x и y, мы будем использовать матрицу.

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


		// Матрица-саммари содержащее результат всех трансформации

		Matrix2D transformMatrix;


		void Update(float deltaTime) override
		{
			// в каждом кадре пересчитываем матрицу
			// порядок важен: ПЕРЕМЕЩЕНИЕ * ПОВОРОТ-ОТВОРОТ * МАСШТАБ

			float rad = rotation * 3.14159f / 180.0f;

			// здесь комбинируем все в одну матрицу
			transformMatrix = Matrix2D::Translation(x, y) * Matrix2D::Rotation(rad);

		}

		
	};
}