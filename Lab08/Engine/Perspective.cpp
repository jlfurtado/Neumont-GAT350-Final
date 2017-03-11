#include "Perspective.h"
#include "Perspective.h"

// Justin Furtadon
// 7/10/2016
// Perspective.h
// Holds data for calculating perspective matrices

namespace Engine
{
	Perspective::Perspective()
		: m_aspectRatio(0.0f), m_nearPlane(0.0f), m_farPlane(0.0f), m_fieldOfViewY(0.0f)
	{
	}

	Perspective::~Perspective()
	{
	}

	void Perspective::SetPerspective(float aspectRatio, float fieldOfViewY, float nearPlane, float farPlane)
	{
		m_aspectRatio = aspectRatio;
		m_fieldOfViewY = fieldOfViewY;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
		m_perspective = Mat4::Perspective(m_fieldOfViewY, m_aspectRatio, m_nearPlane, m_farPlane);
	}

	void Perspective::SetAspectRatio(float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
		m_perspective = Mat4::Perspective(m_fieldOfViewY, m_aspectRatio, m_nearPlane, m_farPlane);
	}

	void Perspective::SetScreenDimmensions(float width, float height)
	{
		m_width = width;
		m_height = height;
		m_perspective = Mat4::Perspective(m_fieldOfViewY, m_aspectRatio, m_nearPlane, m_farPlane);
	}

	float Perspective::GetFOVY()
	{
		return m_fieldOfViewY;
	}

	float Perspective::GetFarDist()
	{
		return m_farPlane;
	}

	float Perspective::GetNearDist()
	{
		return m_nearPlane;
	}

	float Perspective::GetWidth()
	{
		return m_width;
	}

	float Perspective::GetHeight()
	{
		return m_height;
	}

	float Perspective::GetAspectRatio()
	{
		return m_aspectRatio;
	}

	Mat4 Perspective::GetPerspective()
	{
		return m_perspective;
	}

	Mat4 * Perspective::GetPerspectivePtr()
	{
		return &m_perspective;
	}
}
