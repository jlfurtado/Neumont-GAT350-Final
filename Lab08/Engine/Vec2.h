#ifndef VEC2_H
#define VEC2_H

#include "ExportHeader.h"

// Justin Furtado
// 10/12/2016
// Vec2.h
// Vector of two floats for game math

namespace Engine
{
	class ENGINE_SHARED Vec2
	{
	public:
		// constructors
		Vec2();
		Vec2(float v);
		Vec2(float x, float y);
		Vec2(const Vec2& other);

		// destructor
		~Vec2();

		// public methods
		Vec2 Lerp(const Vec2& other, float percent) const;
		Vec2 Normalize() const;
		Vec2 PerpendicularClockwise() const;
		Vec2 PerpendicularCounterClockwise() const;
		Vec2 Projection(const Vec2& other) const;
		Vec2 Rejection(const Vec2& other) const;
		float LengthSquared() const;
		float Length() const;
		float Dot(const Vec2& other) const;
		float CrossMagnitude(const Vec2& other) const;
		Vec2 operator+(const Vec2& other) const;
		Vec2 operator-(const Vec2& other) const;
		float operator*(const Vec2& other) const;
		Vec2 operator-() const;
		Vec2 operator/(float right) const;
		Vec2 operator*(float right) const;
		float operator[](int i) const;
		float GetX() const;
		float GetY() const;
		float *GetAddress();

	private:
		static const int NUM_ELEMENTS = 2;
		float m_values[NUM_ELEMENTS];

	};

	Vec2 ENGINE_SHARED operator*(float left, const Vec2& right);
}

#endif // ifndef VEC2_H