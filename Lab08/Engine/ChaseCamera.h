#ifndef CHASECAMERA_H
#define CHASECAMERA_H

// Justin Furtado
// 7/29/2016
// ChaseCamera.h
// A camera that follows a target

#include "Vec3.h"
#include "Mat4.h"

namespace Engine
{
	class ChaseCamera
	{
	public:
		ChaseCamera();
		ChaseCamera(Vec3 positionOffset, Vec3 targetOffset, Vec3 relativeCameraRotation);
		~ChaseCamera();

		Vec3 GetPosition();
		Vec3 GetTargetPosition();
		Vec3 GetFollowTargetPosition();
		Vec3 GetFollowTargetRotation();
		Vec3 GetPositionOffset();
		Mat4 GetWorldToView();
		void SetPositionOffset(Vec3 offset);
		Vec3 GetTargetOffset();
		void SetTargetOffset(Vec3 offset);
		float GetSpringiness();
		float GetRotateSpeed();
		void SetRotateSpeed(float speed);
		void SetSpringiness(float newSpringiness);
		void Move(Vec3 newFollowTargetPosition, Vec3 newFollowTargetRotation);
		void Rotate(Vec3 deltaRotation);
		void Update(float dt);
		void MouseRotate(int dx, int dy);

	private:
		Vec3 m_positionOffset;
		Vec3 m_targetOffset;
		Vec3 m_relativeCameraRotation;
		Vec3 m_position;
		Vec3 m_targetPosition;
		Vec3 m_followTargetPosition;
		Vec3 m_followTargetRotation;
		float m_springiness;
		float m_rotateSpeed;
		Mat4 m_worldToView;

	};
}

#endif // ifndef CHASECAMERA_H