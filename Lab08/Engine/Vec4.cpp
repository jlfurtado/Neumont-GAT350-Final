#include "Vec4.h"
#include "GameLogger.h"

// Justin Furtado
// 10/12/2016
// Vec4.h
// A vector of 4 floats for games

namespace Engine
{
	Vec4::Vec4()
		: m_values{0.0f, 0.0f, 0.0f, 0.0f}
	{
	}

	Vec4::Vec4(float v)
		: m_values{v, v, v, v}
	{
	}

	Vec4::Vec4(float x, float y, float z, float w)
		:m_values{x, y, z, w}
	{
	}

	Vec4::Vec4(const Vec4 & other)
		: m_values{other.GetX(), other.GetY(), other.GetZ(), other.GetW()}
	{
	}

	Vec4::~Vec4()
	{
	}

	Vec4 Vec4::Lerp(const Vec4 & other, float percent) const
	{
		return ((*this)*(1.0f - percent) + (other*percent));
	}

	Vec4 Vec4::Normalize() const
	{
		float len = Length();
		if (len == 0.0f) { GameLogger::Log(MessageType::Error, "Cannot normalize vector4 of length 0!\n"); return *this; }
		else return *this / len;
	}

	float Vec4::LengthSquared() const
	{
		return ((GetX()*GetX())+(GetY()*GetY())+(GetZ()*GetZ())+(GetW()*GetW()));
	}

	float Vec4::Length() const
	{
		return sqrtf(LengthSquared());
	}

	float Vec4::Dot(const Vec4 & other) const
	{
		return (GetX() * other.GetX()) + (GetY() * other.GetY()) + (GetZ() * other.GetZ()) + (GetW() * other.GetW());
	}

	Vec4 Vec4::operator+(const Vec4 & other) const
	{
		return Vec4{ GetX() + other.GetX(), GetY()  +other.GetY(), GetZ() + other.GetZ(), GetW() + other.GetW() };
	}

	Vec4 Vec4::operator-(const Vec4 & other) const
	{
		return Vec4{GetX() - other.GetX(), GetY() - other.GetY(), GetZ() - other.GetZ(), GetW() - other.GetW()};
	}

	Vec4 Vec4::operator-() const
	{
		return Vec4{-GetX(), -GetY(), -GetZ(), -GetW()};
	}

	Vec4 Vec4::operator/(float right) const
	{
		if (right == 0.0f) { GameLogger::Log(MessageType::Error, "Cannot divide vector4 by zero!\n"); return Vec4{ 0.0f, 0.0f, 0.0f, 0.0f }; }
		return Vec4{ GetX() / right, GetY() / right, GetZ() / right, GetW() / right };
	}

	Vec4 Vec4::operator*(float right) const
	{
		return Vec4{ GetX() * right, GetY() * right, GetX() * right, GetW() * right };
	}

	float Vec4::operator[](int i) const
	{
		if (i < 0 || i > NUM_ELEMENTS) { GameLogger::Log(MessageType::Error, "Index [%d] out of bounds for Vec4!\n", i); return 0.0f; }
		return m_values[i];
	}

	float Vec4::GetX() const
	{
		return m_values[0];
	}

	float Vec4::GetY() const
	{
		return m_values[1];
	}
	
	float Vec4::GetZ() const
	{
		return m_values[2];
	}

	float Vec4::GetW() const
	{
		return m_values[3];
	}

	float Vec4::GetR() const
	{
		return m_values[0];
	}

	float Vec4::GetG() const
	{
		return m_values[1];
	}

	float Vec4::GetB() const
	{
		return m_values[2];
	}

	float Vec4::GetA() const
	{
		return m_values[3];
	}

	float * Vec4::GetAddress()
	{
		return &m_values[0];
	}

	Vec4 operator*(float left, const Vec4 & right)
	{
		return right * left;
	}
}
