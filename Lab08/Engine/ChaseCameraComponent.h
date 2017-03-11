#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

// Justin Furtado
// 8/16/2016
// ChaseCameraComponent.h
// May or may not move around the world

#include "ExportHeader.h"
#include "Component.h"
#include "Vec3.h"
#include "Mat4.h"

namespace Engine
{
	class ENGINE_SHARED ChaseCameraComponent :
		public Component
	{
	public:
		ChaseCameraComponent();
		ChaseCameraComponent(Vec3 positionOffset, Vec3 targetOffset, Vec3 relativeCameraRotation);
		~ChaseCameraComponent();

		bool Initialize() override;
		bool Update(float dt) override;
		Mat4 GetWorldToViewMatrix();
		Mat4 *GetWorldToViewMatrixPtr();
		Vec3 GetUp();
		Vec3 GetViewDir();
		Vec3 GetPosition();
		Vec3 *GetPosPtr();
		Vec3 GetTargetPosition();
		Vec3 GetFollowTargetPosition();
		Vec3 GetFollowTargetRotation();
		Vec3 GetPositionOffset();
		void SetPositionOffset(Vec3 offset);
		Vec3 GetTargetOffset();
		void SetTargetOffset(Vec3 offset);
		float GetSpringiness();
		float GetSpeed();
		float GetRotateSpeed();
		float GetDistanceMultiplier();
		void SetDistanceMultiplier(float value);
		void SetSpeed(float newSpeed);
		void SetRotateSpeed(float speed);
		void SetSpringiness(float newSpringiness);
		void Move(Vec3 newFollowTargetPosition, Vec3 newFollowTargetRotation);
		void Rotate(Vec3 deltaRotation);
		void MouseRotate(int dx, int dy);

	private:
		Vec3 m_up;
		Vec3 m_viewDir;
		Vec3 m_positionOffset;
		Vec3 m_targetOffset;
		Vec3 m_relativeCameraRotation;
		Vec3 m_position;
		Vec3 m_targetPosition;
		Vec3 m_followTargetPosition;
		Vec3 m_followTargetRotation;
		float m_distanceMultiplier;
		float m_currentDistanceMultiplier;
		float m_springiness;
		float m_rotateSpeed;
		float m_moveSpeed;
		Mat4 m_worldToView;
	};
}

#endif // ifndef CAMERACOMPONENT_H