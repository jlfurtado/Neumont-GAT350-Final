#include "Mat4.h"
#include "Mat4.h"
#include <math.h>
#include "GameLogger.h"

// Justin Furtado
// 10/12/2016
// Mat4.cpp
// A Matrix4c4 class for transforming vectors

namespace Engine
{
	Mat4::Mat4()
		: m_values{1.0f, 0.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f, 0.0f,
				   0.0f, 0.0f, 1.0f, 0.0f,
				   0.0f, 0.0f, 0.0f, 1.0f}
	{
	}

	Mat4::Mat4(const Vec3 & left, const Vec3 & middle, const Vec3 & right)
		: m_values{left.GetX(), middle.GetX(), right.GetX(), 0.0f,
				   left.GetY(), middle.GetY(), right.GetY(), 0.0f,
				   left.GetZ(), middle.GetZ(), right.GetZ(), 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f}
	{
	}

	Mat4::Mat4(const Vec4 & left, const Vec4 & middleLeft, const Vec4 middleRight, const Vec4 & right)
		: m_values{left.GetX(), middleLeft.GetX(), middleRight.GetX(), right.GetX(),
				   left.GetY(), middleLeft.GetY(), middleRight.GetY(), right.GetY(),
				   left.GetZ(), middleLeft.GetZ(), middleRight.GetZ(), right.GetZ(),
				   left.GetW(), middleLeft.GetW(), middleRight.GetW(), right.GetW()}
	{
	}

	Mat4::Mat4(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11, float f12, float f13, float f14, float f15)
		: m_values{f0, f4, f8, f12,
				   f1, f5, f9, f13,
				   f2, f6, f10, f14,
				   f3, f7, f11, f15}
	{
	}

	Mat4::~Mat4()
	{
	}

	Mat4 Mat4::RotationAroundAxis(const Vec3& axisToRotateAbout, float radians)
	{
		Vec3 axis = axisToRotateAbout.Normalize();
		float s = sinf(radians);
		float c = cosf(radians);
		float oc = 1.0f - c;
		
		return Mat4(oc * axis.GetX() * axis.GetX() + c, oc * axis.GetX() * axis.GetY() - axis.GetZ() * s, oc * axis.GetZ() * axis.GetX() + axis.GetY() * s, 0.0f,
					oc * axis.GetX() * axis.GetY() + axis.GetZ() * s, oc * axis.GetY() * axis.GetY() + c, oc * axis.GetY() * axis.GetZ() - axis.GetX() * s, 0.0f,
					oc * axis.GetZ() * axis.GetX() - axis.GetY() * s, oc * axis.GetY() * axis.GetZ() + axis.GetX() * s, oc * axis.GetZ() * axis.GetZ() + c, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);

	}

	Mat4 Mat4::Scale(float scale)
	{
		return Mat4(scale, 0.0f, 0.0f, 0.0f,
					0.0f, scale, 0.0f, 0.0f,
					0.0f, 0.0f, scale, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	Mat4 Mat4::Scale(float xScale, float yScale, float zScale)
	{
		return Mat4(xScale, 0.0f, 0.0f, 0.0f,
					0.0f, yScale, 0.0f, 0.0f,
					0.0f, 0.0f, zScale, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	Mat4 Mat4::Translation(float x, float y, float z)
	{
		return Mat4(1.0f, 0.0f, 0.0f, x,
					0.0f, 1.0f, 0.0f, y,
					0.0f, 0.0f, 1.0f, z,
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	Mat4 Mat4::Translation(const Vec3 & translationVector)
	{
		return Translation(translationVector.GetX(), translationVector.GetY(), translationVector.GetZ());
	}

	Mat4 Mat4::Perspective(float fieldOfViewY, float aspectRatio, float nearPlane, float farPlane)
	{
		if (fieldOfViewY <= 0.0f || aspectRatio == 0.0f) { GameLogger::Log(MessageType::Error, "Invalid perspective matrix! fov = [%.3f], aspect = [%.3f]", fieldOfViewY, aspectRatio); }

		float f = 1.0f / tanf(0.5f*fieldOfViewY);
		float depth = farPlane - nearPlane;
		float oneOverDepth = 1.0f / depth;

		return Mat4(-f/aspectRatio, 0.0f, 0.0f, 0.0f,
					0.0f, f, 0.0f, 0.0f,
					0.0f, 0.0f, farPlane*oneOverDepth, -farPlane*nearPlane*oneOverDepth,
					0.0f, 0.0f, 1.0f, 0.0f);
	}

	Mat4 Mat4::Orthographic(float left, float right, float top, float bottom)
	{
		return Mat4(
			2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
			0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 1.0f
		);
	}

	Mat4 Mat4::InfinitePerspective(float fovy, float aspect, float near)
	{
		float range = tan(fovy * 0.5f) * near;
		float left = -range * aspect;
		float right = range * aspect;
		float bottom = -range;
		float top = range;

		return Mat4((2.0f * near) / (right - left), 0.0f, 0.0f, 0.0f,
					0.0f, (2.0f * near) / (top - bottom), 0.0f, 0.0f,
					0.0f, 0.0f, -1.0f, -1.0f,
					0.0f, 0.0f, -2.0f * near, 0.0f);
	}


	Mat4 Mat4::LookAt(const Vec3 & cameraPosition, const Vec3 & targetPosition, const Vec3 & up)
	{
		Vec3 z = (targetPosition - cameraPosition).Normalize();
		Vec3 x = (up.Cross(z)).Normalize();
		Vec3 y = z.Cross(x);

		return Mat4(x.GetX(), x.GetY(), x.GetZ(), -x.Dot(cameraPosition),
					y.GetX(), y.GetY(), y.GetZ(), -y.Dot(cameraPosition),
					z.GetX(), z.GetY(), z.GetZ(), -z.Dot(cameraPosition),
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	Mat4 Mat4::ViewPort(float right, float left, float top, float bottom, float far, float near)
	{
		return Mat4(0.5f * (right - left), 0.0f, 0.0f, 0.5f * (right + left),
					0.0f, 0.5f * (top - bottom), 0.0f, 0.5f * (top + bottom),
					0.0f, 0.0f, 0.5f * (far - near), 0.5f * (far + near),
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	Mat4 Mat4::Bias()
	{
		return Mat4(0.5f, 0.0f, 0.0f, 0.5f,
					0.0f, 0.5f, 0.0f, 0.5f,
					0.0f, 0.0f, 0.5f, 0.5f,
					0.0f, 0.0f, 0.0f, 1.0f);
	}

	float * Mat4::GetAddress()
	{
		return &m_values[0];
	}

	Mat4 Mat4::operator*(const Mat4 & right) const
	{		
		/*
			|0 4 8  12 |   |0 4 8  12 |
			|1 5 9  13 |   |1 5 9  13 |
			|2 6 10 14 |   |2 6 10 14 |
			|3 7 11 15 | * |3 7 11 15 |*/
		return Mat4(Vec4(m_values[0], m_values[4], m_values[8], m_values[12]).Dot(Vec4(right.m_values[0],  right.m_values[1],  right.m_values[2],  right.m_values[3] )),
					Vec4(m_values[0], m_values[4], m_values[8], m_values[12]).Dot(Vec4(right.m_values[4], right.m_values[5], right.m_values[6], right.m_values[7])),
					Vec4(m_values[0], m_values[4], m_values[8], m_values[12]).Dot(Vec4(right.m_values[8], right.m_values[9], right.m_values[10], right.m_values[11])),
					Vec4(m_values[0], m_values[4], m_values[8], m_values[12]).Dot(Vec4(right.m_values[12], right.m_values[13], right.m_values[14], right.m_values[15])),

					Vec4(m_values[1], m_values[5], m_values[9], m_values[13]).Dot(Vec4(right.m_values[0],  right.m_values[1],  right.m_values[2],  right.m_values[3] )),
					Vec4(m_values[1], m_values[5], m_values[9], m_values[13]).Dot(Vec4(right.m_values[4], right.m_values[5], right.m_values[6], right.m_values[7])),
					Vec4(m_values[1], m_values[5], m_values[9], m_values[13]).Dot(Vec4(right.m_values[8], right.m_values[9], right.m_values[10], right.m_values[11])),
					Vec4(m_values[1], m_values[5], m_values[9], m_values[13]).Dot(Vec4(right.m_values[12], right.m_values[13], right.m_values[14], right.m_values[15])),

					Vec4(m_values[2], m_values[6], m_values[10], m_values[14]).Dot(Vec4(right.m_values[0],  right.m_values[1],  right.m_values[2],  right.m_values[3] )),
					Vec4(m_values[2], m_values[6], m_values[10], m_values[14]).Dot(Vec4(right.m_values[4], right.m_values[5], right.m_values[6], right.m_values[7])),
					Vec4(m_values[2], m_values[6], m_values[10], m_values[14]).Dot(Vec4(right.m_values[8], right.m_values[9], right.m_values[10], right.m_values[11])),
					Vec4(m_values[2], m_values[6], m_values[10], m_values[14]).Dot(Vec4(right.m_values[12], right.m_values[13], right.m_values[14], right.m_values[15])),

					Vec4(m_values[3], m_values[7], m_values[11], m_values[15]).Dot(Vec4(right.m_values[0],  right.m_values[1],  right.m_values[2],  right.m_values[3] )),
					Vec4(m_values[3], m_values[7], m_values[11], m_values[15]).Dot(Vec4(right.m_values[4],  right.m_values[5],  right.m_values[6],  right.m_values[7] )), 
					Vec4(m_values[3], m_values[7], m_values[11], m_values[15]).Dot(Vec4(right.m_values[8],  right.m_values[9],  right.m_values[10], right.m_values[11])), 
					Vec4(m_values[3], m_values[7], m_values[11], m_values[15]).Dot(Vec4(right.m_values[12], right.m_values[13], right.m_values[14], right.m_values[15])));
	}

	Vec3 Mat4::operator*(const Vec3 & right) const
	{
		/*
		|0 4 8  12 |   |0|
		|1 5 9  13 |   |1|
		|2 6 10 14 |   |2|
		|3 7 11 15 | * |3|*/
		return Vec3(Vec4(m_values[0], m_values[4], m_values[8], m_values[12]).Dot(Vec4(right.GetX(), right.GetY(), right.GetZ(), 1.0f)),
					Vec4(m_values[1], m_values[5], m_values[9], m_values[13]).Dot(Vec4(right.GetX(), right.GetY(), right.GetZ(), 1.0f)),
					Vec4(m_values[2], m_values[6], m_values[10], m_values[14]).Dot(Vec4(right.GetX(), right.GetY(), right.GetZ(), 1.0f)));
	}

	Vec4 Mat4::operator*(const Vec4 & right) const
	{
		return Vec4(Vec4(m_values[0],  m_values[4],  m_values[8] , m_values[12]).Dot(right),
					Vec4(m_values[1],  m_values[5],  m_values[9] , m_values[13]).Dot(right),
					Vec4(m_values[2],  m_values[6],  m_values[10], m_values[14]).Dot(right),
					Vec4(m_values[3],  m_values[7],  m_values[11], m_values[15]).Dot(right));
	}

	float& Mat4::operator[](unsigned index)
	{
		return m_values[index];
	}
}
