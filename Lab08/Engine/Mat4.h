#ifndef MAT4_H
#define MAT4_H

// Justin Furtado
// 10/13/2016
// Mat4.h
// A Matrix4c4 class for transforming vectors

#include "ExportHeader.h"
#include "Vec3.h"
#include "Vec4.h"

namespace Engine
{
	class ENGINE_SHARED Mat4
	{
	public:
		// constructors
		Mat4();
		Mat4(const Vec3& left, const Vec3& middle, const Vec3& right);
		Mat4(const Vec4& left, const Vec4& middleLeft, const Vec4 middleRight, const Vec4& right);
		Mat4(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11, float f12, float f13, float f14, float f15);
		
		//destructor
		~Mat4();

		// public static methods that return matrices
		static Mat4 RotationAroundAxis(const Vec3& axisToRotateAbout, float radians);
		static Mat4 Scale(float scale);
		static Mat4 Scale(float xScale, float yScale, float zScale);
		static Mat4 Translation(float x, float y, float z);
		static Mat4 Translation(const Vec3& translationVector);
		static Mat4 Perspective(float fieldOfViewY, float aspectRatio, float nearPlane, float farPlane);
		static Mat4 Orthographic(float left, float right, float top, float bottom);
		static Mat4 LookAt(const Vec3& cameraPosition, const Vec3& targetPosition, const Vec3& up);
		static Mat4 ViewPort(float right, float left, float top, float bottom, float far, float near);
		static Mat4 Bias();
		static Mat4 InfinitePerspective(float fovy, float aspect, float near);

		// utility
		float * GetAddress();

		// operator overloads
		Mat4 operator*(const Mat4& right) const;
		Vec3 operator*(const Vec3& right) const;
		Vec4 operator*(const Vec4& right) const;

		float& operator[](unsigned index);
	private:
		static const int ELEMENTS_PER_DIRECTION = 4;
		static const int TOTAL_ELEMENTS = ELEMENTS_PER_DIRECTION * ELEMENTS_PER_DIRECTION;
		float m_values[TOTAL_ELEMENTS];
	};
}

#endif // ifndef MAT4_H