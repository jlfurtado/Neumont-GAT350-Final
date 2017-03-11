#include "MousePicker.h"
#include <math.h>

// Justin Furtado
// 11/1/2016
// MousePicker.cpp
// Handles mouse selection of three dimmensional objects

namespace Engine
{
	Vec3 MousePicker::s_cameraPosition;
	Vec3 MousePicker::s_horizontalDirection;
	Vec3 MousePicker::s_verticalDirection;
	Vec3 MousePicker::s_viewDirection;
	float MousePicker::s_horizontalLength = 0.0f;
	float MousePicker::s_verticalLength = 0.0f;
	float MousePicker::s_nearClipDistance = 0.0f;
	float MousePicker::s_screenHeight = 0.0f;
	float MousePicker::s_screenWidth = 0.0f;

	void MousePicker::SetCameraInfo(Engine::Vec3 cameraPosition, Engine::Vec3 cameraViewDir, Engine::Vec3 cameraUp)
	{
		s_cameraPosition = cameraPosition;
		s_viewDirection = cameraViewDir.Normalize();
		s_horizontalDirection = s_viewDirection.Cross(cameraUp).Normalize();
		s_verticalDirection = s_horizontalDirection.Cross(s_viewDirection).Normalize();
	}

	void MousePicker::SetPerspectiveInfo(float fovyRadians, float nearClipDist, float screenWidth, float screenHeight)
	{
		s_screenWidth = screenWidth;
		s_screenHeight = screenHeight;
		s_nearClipDistance = nearClipDist;
		s_verticalLength = tanf(fovyRadians / 2.0f) * nearClipDist;
		s_horizontalLength = s_verticalLength * (screenWidth / screenHeight);
	}

	Vec3 MousePicker::GetOrigin(int pixelX, int pixelY)
	{
		float x = (pixelX - s_screenWidth / 2.0f) / (s_screenWidth / 2.0f);
		float y = -(pixelY - s_screenHeight / 2.0f) / (s_screenHeight / 2.0f);
		return s_cameraPosition + s_viewDirection * s_nearClipDistance + (x*s_horizontalDirection*s_horizontalLength) + (y*s_verticalDirection*s_verticalLength);
	}

	Vec3 MousePicker::GetDirection(int pixelX, int pixelY)
	{
		return GetOrigin(pixelX, pixelY) - s_cameraPosition;
	}

}