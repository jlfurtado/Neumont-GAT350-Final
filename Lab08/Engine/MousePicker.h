#ifndef MOUSEPICKER_H
#define MOUSEPICKER_H

// Justin Furtado
// 11/1/2016
// MousePicker.h
// Handles mouse selection of three dimmensional objects

#include "Vec3.h"
#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED MousePicker
	{
	public:
		void static SetCameraInfo(Engine::Vec3 cameraPosition, Engine::Vec3 cameraViewDir, Engine::Vec3 cameraUp);
		void static SetPerspectiveInfo(float fovyRadians, float nearClipDist, float screenWidth, float screenHeight);
		Vec3 static GetOrigin(int pixelX, int pixelY);
		Vec3 static GetDirection(int pixelX, int pixelY);

	private:
		static Vec3 s_cameraPosition;
		static Vec3 s_viewDirection;
		static Vec3 s_horizontalDirection;
		static Vec3 s_verticalDirection;
		static float s_horizontalLength;
		static float s_verticalLength;
		static float s_nearClipDistance;
		static float s_screenWidth;
		static float s_screenHeight;
	};
}

#endif // ifndef MOUSEPICKER_H