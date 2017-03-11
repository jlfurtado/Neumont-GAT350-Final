#ifndef VEC4_H
#define VEC4_H

// Justin Furtado
// 10/12/2016
// Vec4.h
// A vector of 4 floats for games

#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED Vec4
	{
	public:
		// constructors
		Vec4();
		Vec4(float v);
		Vec4(float x, float y, float z, float w);
		Vec4(const Vec4& other);

		// destructor
		~Vec4();

		// public methods
		Vec4 Lerp(const Vec4& other, float percent) const;
		Vec4 Normalize() const;
		// Projection?
		// Rejection?
		float LengthSquared() const;
		float Length() const;
		float Dot(const Vec4& other) const;
		//Vec4 Cross(const Vec4& other) const;
		Vec4 operator+(const Vec4& other) const;
		Vec4 operator-(const Vec4& other) const;
		Vec4 operator-() const;
		Vec4 operator/(float right) const;
		Vec4 operator*(float right) const;
		float operator[](int i) const;
		float GetX() const;
		float GetY() const;
		float GetZ() const;
		float GetW() const;
		float GetR() const;
		float GetG() const;
		float GetB() const;
		float GetA() const;
		float *GetAddress();

	private:
		static const int NUM_ELEMENTS = 4;
		float m_values[NUM_ELEMENTS];

	};

	Vec4 ENGINE_SHARED operator*(float left, const Vec4& right);
}

#endif // ifndef VEC4_h