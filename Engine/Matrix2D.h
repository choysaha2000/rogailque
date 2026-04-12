#pragma once
#include <cmath>



namespace XYZengine
{
	class Matrix2D
	{
	public:
		float m[3][3];

		Matrix2D() { Identity(); }

		// делает матрицу единичной (сброс)

		void Identity()
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					m[i][j] = (i == j) ? 1.0f : 0.0f;
				}
			}
		}

		// создание матрицы перемещения

		static Matrix2D Translation(float x, float y)
		{
			Matrix2D result;
			result.m[0][2] = x;
			result.m[1][2] = y;
			return result;
		}

		// создание матрицы поворота (угол в радианах)

		static Matrix2D Rotation(float angle)
		{
			Matrix2D result;
			result.m[0][0] = cos(angle);
			result.m[0][1] = -sin(angle);
			result.m[1][0] = sin(angle);
			result.m[1][1] = cos(angle);
			return result;
		}

		// произведение матриц 

		Matrix2D operator* (const Matrix2D& other) const
		{
			Matrix2D result;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					result.m[i][j] = m[i][0] * other.m[0][j] +
						m[i][1] * other.m[1][j] +
						m[i][2] * other.m[2][j];
				}
			}

			return result;
		}

	};
}