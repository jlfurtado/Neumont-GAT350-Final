#include "Vec2.h"
#include <math.h>
#include "GameLogger.h"

// Justin Furtado
// 10/12/2016
// Vec2.h
// Vector of two floats for game math

namespace Engine
{
	Vec2::Vec2()
		: m_values{ 0.0f, 0.0f }
	{
	}

	Vec2::Vec2(float v)
		: m_values{ v, v }
	{
	}

	Vec2::Vec2(float x, float y)
		: m_values{ x, y }
	{
	}

	Vec2::Vec2(const Vec2& other)
		:m_values{ other.GetX(), other.GetY() }
	{
	}

	Vec2::~Vec2()
	{
	}

	Vec2 Vec2::Lerp(const Vec2 & other, float percent) const
	{
		return ((*this)*(1.0f-percent) + (other*percent));
	}

	Vec2 Vec2::Normalize() const
	{
		float len = Length();
		if (len == 0.0f) { GameLogger::Log(MessageType::Error, "Cannot normalize vector2 of length 0!\n"); return *this; }
		else return *this / len;
	}

	Vec2 Vec2::PerpendicularClockwise() const
	{
		return Vec2{ -GetX(), GetY() };
	}

	Vec2 Vec2::PerpendicularCounterClockwise() const
	{
		return Vec2{ GetX(), -GetY() };
	}

	Vec2 Vec2::Projection(const Vec2 & other) const
	{
		float lenSquared = other.LengthSquared();
		if (lenSquared == 0.0f) { GameLogger::Log(MessageType::Error, "Cannot calculate projection of length 0 vector!\n"); return *this; }
		else return (other * (Dot(other))) / lenSquared;
	}

	Vec2 Vec2::Rejection(const Vec2 & other) const
	{
		return (*this) - Projection(other);
	}

	float Vec2::LengthSquared() const
	{
		return (GetX() * GetX()) + (GetY() * GetY());
	}

	float Vec2::Length() const
	{
		return sqrtf(LengthSquared());
	}

	float Vec2::Dot(const Vec2 & other) const
	{
		return (GetX() * other.GetX()) + (GetY() * other.GetY());
	}

	float Vec2::CrossMagnitude(const Vec2 & other) const
	{
		return (GetX() * other.GetY()) - (GetY() * other.GetX());
	}

	Vec2 Vec2::operator+(const Vec2 & other) const
	{
		return Vec2{ GetX() + other.GetX(), GetY() + other.GetY() };
	}

	Vec2 Vec2::operator-(const Vec2 & other) const
	{
		return Vec2{ GetX() - other.GetX(), GetY() - other.GetY() };
	}

	float Vec2::operator*(const Vec2 & other) const
	{
		return Dot(other);
	}

	Vec2 Vec2::operator-() const
	{
		return Vec2{ -GetX(), -GetY() };
	}

	Vec2 Vec2::operator/(float right) const
	{
		if (right == 0.0f) { GameLogger::Log(MessageType::Error, "Cannot divide vector2 by zero!\n"); return Vec2{ 0.0f, 0.0f }; }
		else return Vec2{ GetX() / right, GetY() / right };
	}

	Vec2 Vec2::operator*(float right) const
	{
		return Vec2{ GetX() * right, GetY() * right };
	}

	float Vec2::operator[](int i) const
	{
		if (i < 0 || i > NUM_ELEMENTS) { GameLogger::Log(MessageType::Error, "Index [%d] out of bounds for Vec2!\n", i); return 0.0f; }
		return m_values[i];
	}

	float Vec2::GetX() const
	{
		return m_values[0];
	}

	float Vec2::GetY() const
	{
		return m_values[1];
	}

	float *Vec2::GetAddress()
	{
		return &m_values[0];
	}

	Vec2 operator*(float left, const Vec2 & right)
	{
		return right * left;
	}

}
