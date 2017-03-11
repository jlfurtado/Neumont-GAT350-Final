#include "Mat3.h"
#include <math.h>

// Justin Furtado
// 10/12/2016
// Mat3.cpp
// A Matrix3x3 class for transforming vectors

// TODO: FIX LIKE MAT4

namespace Engine
{
	Mat3::Mat3()
		: m_values{1.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f,
				   0.0f, 0.0f, 1.0f}
	{
	}

	Mat3::Mat3(const Vec2 & left, const Vec2 & right)
		: m_values{left.GetX(), right.GetX(), 0.0f,
				   left.GetY(), right.GetY(), 0.0f,
					0.0f, 0.0f, 1.0f}
	{
	}

	Mat3::Mat3(const Vec3 & left, const Vec3 & middle, const Vec3 & right)
		: m_values{left.GetX(), middle.GetX(), right.GetX(),
				   left.GetY(), middle.GetY(), right.GetY(),
				   left.GetZ(), middle.GetZ(), right.GetZ()}
	{
	}

	Mat3::Mat3(float topLeft, float topMiddle, float topRight, float middleLeft, float middleMiddle, float middleRight, float bottomLeft, float bottomMiddle, float bottomRight)
		: m_values{topLeft, topMiddle, topRight,
				   middleLeft, middleMiddle, middleRight,
				   bottomLeft, bottomMiddle, bottomRight}
	{
	}

	Mat3::~Mat3()
	{
	}

	Mat3 Mat3::Rotation(float radians)
	{
		return Mat3(cosf(radians), -sinf(radians), 0.0f,
					sinf(radians), cosf(radians), 0.0f,
					0.0f, 0.0f, 1.0f);

	}

	Mat3 Mat3::Scale(float scale)
	{
		return Mat3(scale, 0.0f, 0.0f,
					0.0f, scale, 0.0f,
					0.0f, 0.0f, 1.0f);
	}

	Mat3 Mat3::Scale(float xScale, float yScale)
	{
		return Mat3(xScale, 0.0f, 0.0f,
					0.0f, yScale, 0.0f,
					0.0f, 0.0f, 1.0f);
	}

	Mat3 Mat3::Translation(float x, float y)
	{
		return Mat3(1.0f, 0.0f, x,
					0.0f, 1.0f, y,
					0.0f, 0.0f, 1.0f);
	}
	
	Mat3 Mat3::Translation(const Vec2 & translationVector)
	{
		return Mat3(1.0f, 0.0f, translationVector.GetX(),
					0.0f, 1.0f, translationVector.GetY(),
					0.0f, 0.0f, 1.0f);
	}
	
	float * Mat3::GetAddress()
	{
		return &m_values[0];
	}
	
	Mat3 Mat3::operator*(const Mat3 & right) const
	{
		return Mat3(Vec3(m_values[0], m_values[1], m_values[2]).Dot(Vec3(right.m_values[0], right.m_values[3], right.m_values[6])),
					Vec3(m_values[0], m_values[1], m_values[2]).Dot(Vec3(right.m_values[1], right.m_values[4], right.m_values[7])),
					Vec3(m_values[0], m_values[1], m_values[2]).Dot(Vec3(right.m_values[2], right.m_values[5], right.m_values[8])),
					Vec3(m_values[3], m_values[4], m_values[5]).Dot(Vec3(right.m_values[0], right.m_values[3], right.m_values[6])),
					Vec3(m_values[3], m_values[4], m_values[5]).Dot(Vec3(right.m_values[1], right.m_values[4], right.m_values[7])),
					Vec3(m_values[3], m_values[4], m_values[5]).Dot(Vec3(right.m_values[2], right.m_values[5], right.m_values[8])),
					Vec3(m_values[6], m_values[7], m_values[8]).Dot(Vec3(right.m_values[0], right.m_values[3], right.m_values[6])),
					Vec3(m_values[6], m_values[7], m_values[8]).Dot(Vec3(right.m_values[1], right.m_values[4], right.m_values[7])),
					Vec3(m_values[6], m_values[7], m_values[8]).Dot(Vec3(right.m_values[2], right.m_values[5], right.m_values[8])));
	}
	
	Vec2 Mat3::operator*(const Vec2 & right) const
	{
		return Vec2(Vec3(m_values[0], m_values[1], m_values[2]).Dot(Vec3(right.GetX(), right.GetY(), 1.0f)),
					Vec3(m_values[3], m_values[4], m_values[5]).Dot(Vec3(right.GetX(), right.GetY(), 1.0f)));
	}
	
	Vec3 Mat3::operator*(const Vec3 & right) const
	{
		return Vec3(Vec3(m_values[0], m_values[1], m_values[2]).Dot(right),
					Vec3(m_values[3], m_values[4], m_values[5]).Dot(right),
					Vec3(m_values[6], m_values[7], m_values[8]).Dot(right));
	}
}
