#include "Mat2.h"
#include <math.h>

// Justin Furtado
// Mat2.cpp
// 10/12/2016
// Matrix2x2 class for transforming vectors

// TODO: FIX LIKE MAT4

namespace Engine
{
	Mat2::Mat2()
		: m_values{ 1.0f, 0.0f, 0.0f, 1.0f }
	{
	}

	Mat2::Mat2(const Vec2 & leftColumnVector, const Vec2 & rightColumnVector)
		: m_values{leftColumnVector.GetX(), rightColumnVector.GetX(), leftColumnVector.GetY(), rightColumnVector.GetY() }
	{
	}

	Mat2::Mat2(float topLeft, float topRight, float bottomLeft, float bottomRight)
		: m_values{topLeft, topRight, bottomLeft, bottomRight}
	{
	}

	Mat2::~Mat2()
	{
	}

	Mat2 Mat2::Rotate(float radians)
	{
		return Mat2(Vec2(cosf(radians), sinf(radians)), Vec2(-sinf(radians), cosf(radians)));
	}

	Mat2 Mat2::Scale(float uniformScale)
	{
		return Mat2(Vec2(uniformScale, 0.0f), Vec2(0.0f, uniformScale));
	}

	Mat2 Mat2::Scale(float xScale, float yScale)
	{
		return Mat2(Vec2(xScale, 0.0f), Vec2(0.0f, yScale));
	}

	float * Mat2::GetAddress()
	{
		return &m_values[0];
	}

	Mat2 Mat2::operator*(const Mat2 & right) const
	{
		/*
		|a00 a10|	|b00 b10|   |a00*b00+a10*b01 a00*b10+a10*b11|
		|a01 a11| * |b01 b11| = |a01*b00+a11*b01 a01*b10+a11*b11|
		*/
		return Mat2(Vec2(Vec2(m_values[0], m_values[1]).Dot(Vec2(right.m_values[0], right.m_values[2])),
						 Vec2(m_values[2], m_values[3]).Dot(Vec2(right.m_values[0], right.m_values[2]))),
					Vec2(Vec2(m_values[0], m_values[1]).Dot(Vec2(right.m_values[1], right.m_values[3])),
						 Vec2(m_values[2], m_values[3]).Dot(Vec2(right.m_values[1], right.m_values[3]))));
	}

	Vec2 Mat2::operator*(const Vec2 & right) const
	{
		/*
		| a00 a10 |   | b00 |   | a00*b00+a10*b01 |
		| a01 a11 | * | b01 | = | a01*b00+a11*b01 |
		*/
		return Vec2(Vec2(m_values[0], m_values[1]).Dot(right), Vec2(m_values[2], m_values[3]).Dot(right));
	}
}
