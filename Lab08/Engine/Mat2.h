#ifndef MAT2_H
#define MAT2_H

// Justin Furtado
// Mat2.h
// 10/12/2016
// Matrix2x2 class for transforming vectors

#include "ExportHeader.h"
#include "Vec2.h"

namespace Engine
{
	class ENGINE_SHARED Mat2
	{
	public:
		// constructors
		Mat2();
		Mat2(const Vec2& leftColumnVector, const Vec2& rightColumnVector);
		Mat2(float topLeft, float topRight, float bottomLeft, float bottomRight);

		// destructor
		~Mat2();

		// matrix creation methods
		static Mat2 Rotate(float radians);
		static Mat2 Scale(float uniformScale);
		static Mat2 Scale(float xScale, float yScale);

		// utility
		float *GetAddress();

		// operators
		Mat2 operator*(const Mat2& right) const;
		Vec2 operator*(const Vec2& right) const;

	private:
		static const int ELEMENTS_PER_DIRECTION = 2;
		static const int TOTAL_ELEMENTS = ELEMENTS_PER_DIRECTION * ELEMENTS_PER_DIRECTION;
		float m_values[TOTAL_ELEMENTS];
	};
}

#endif // ifndef MAT2_H