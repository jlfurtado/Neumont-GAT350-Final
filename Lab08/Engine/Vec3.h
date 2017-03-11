#ifndef VEC3_H
#define VEC3_H

#include "ExportHeader.h"

// Justin Furtado
// 10/12/2016
// Vec3.h
// A vector of 3 floats

namespace Engine
{
	class ENGINE_SHARED Vec3
	{
	public:
		// constructors
		Vec3();
		Vec3(float v);
		Vec3(float x, float y, float z);
		Vec3(const Vec3& other);

		// destructor
		~Vec3();

		// public methods
		Vec3 Lerp(const Vec3& other, float percent) const;
		Vec3 Normalize() const;
		// Projection?
		// Rejection?
		float LengthSquared() const;
		float Length() const;
		float Dot(const Vec3& other) const;
		Vec3 Cross(const Vec3& other) const;
		Vec3 operator+(const Vec3& other) const;
		Vec3 operator-(const Vec3& other) const;
		Vec3 operator-() const;
		Vec3 operator/(float right) const;
		Vec3 operator*(float right) const;
		float operator[](int i) const;
		float GetX() const;
		float GetY() const;
		float GetZ() const;
		float GetR() const;
		float GetG() const;
		float GetB() const;
		float *GetAddress();

	private:
		static const int NUM_ELEMENTS = 3;
		float m_values[NUM_ELEMENTS];
	};

	Vec3 ENGINE_SHARED operator*(float left, const Vec3& right);
}
#endif // ifndef VEC3_h