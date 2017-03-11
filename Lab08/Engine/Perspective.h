#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

// Justin Furtado
// 7/10/2016
// Perspective.h
// Holds data for calculating perspective matrices

#include "ExportHeader.h"
#include "Mat4.h"

namespace Engine
{
	class ENGINE_SHARED Perspective
	{
	public:
		// ctor/dtor
		Perspective();
		~Perspective();

		// methods
		void SetPerspective(float aspectRatio, float fieldOfViewY, float nearPlane, float farPlane);
		void SetAspectRatio(float aspectRatio);
		void SetScreenDimmensions(float width, float height);
		float GetFOVY();
		float GetFarDist();
		float GetNearDist();
		float GetWidth();
		float GetHeight();
		float GetAspectRatio();
		Mat4 GetPerspective();
		Mat4 *GetPerspectivePtr();

	private:
		// data
		float m_width;
		float m_height;
		float m_aspectRatio;
		float m_fieldOfViewY;
		float m_nearPlane;
		float m_farPlane;
		Mat4 m_perspective;
	};
}

#endif // ifndef PERSPECTIVE_H