#ifndef MAT3_H
#define MAT3_H

// Justin Furtado
// 10/12/2016
// Mat3.h
// A Matrix3x3 class for transforming vectors

#include "ExportHeader.h"
#include "Vec3.h"
#include "Vec2.h"

namespace Engine
{
	class ENGINE_SHARED Mat3
	{
	public:
		// constructors
		Mat3();
		Mat3(const Vec2& left, const Vec2& right);
		Mat3(const Vec3& left, const Vec3& middle, const Vec3& right);
		Mat3(float topLeft, float topMiddle, float topRight, float middleLeft, float middleMiddle, float middleRight, float bottomLeft, float bottomMiddle, float bottomRight);

		//destructor
		~Mat3();

		// public static methods that return matrices
		static Mat3 Rotation(float radians);
		static Mat3 Scale(float scale);
		static Mat3 Scale(float xScale, float yScale);
		static Mat3 Translation(float x, float y);
		static Mat3 Translation(const Vec2& translationVector);

		// utility
		float * GetAddress();

		// operator overloads
		Mat3 operator*(const Mat3& right) const;
		Vec2 operator*(const Vec2& right) const;
		Vec3 operator*(const Vec3& right) const;

	private:
		static const int ELEMENTS_PER_DIRECTION = 3;
		static const int TOTAL_ELEMENTS = ELEMENTS_PER_DIRECTION * ELEMENTS_PER_DIRECTION;
		float m_values[TOTAL_ELEMENTS];
	};
}

#endif // ifndef MAT3_H