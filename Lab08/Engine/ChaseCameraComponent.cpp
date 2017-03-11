#include "ChaseCameraComponent.h"
#include "ChaseCameraComponent.h"
#include "ChaseCameraComponent.h"
#include "SpatialComponent.h"
#include "MathUtility.h"
#include "CollisionTester.h"

// Justin Furtado
// 8/16/2016
// ChaseCameraComponent.h
// May or may not move around the world

namespace Engine
{
	ChaseCameraComponent::ChaseCameraComponent()
		: m_springiness(0.15f), m_rotateSpeed(1.0f), m_moveSpeed(150.0f)
	{
	}

	ChaseCameraComponent::ChaseCameraComponent(Vec3 positionOffset, Vec3 targetOffset, Vec3 relativeCameraRotation)
		: m_positionOffset(positionOffset), m_targetOffset(targetOffset),
		m_relativeCameraRotation(relativeCameraRotation),
		m_springiness(0.15f), m_rotateSpeed(1.0f)
	{
	}

	ChaseCameraComponent::~ChaseCameraComponent()
	{
	}

	bool ChaseCameraComponent::Initialize()
	{
		GameLogger::Log(MessageType::Process, "Camera Component [%s] initialized successfully!\n", GetName());
		return true;
	}

	bool ChaseCameraComponent::Update(float /*dt*/)
	{
		// need spatial component to do maths
		SpatialComponent *pSpatial = GetSiblingComponent<SpatialComponent>();
		if (!pSpatial) { return true; }
		
		// calc rotation matrix for camera
		Mat4 y = Mat4::RotationAroundAxis(Vec3(0.0f, 1.0f, 0.0f), m_relativeCameraRotation.GetY());
		Mat4 p = Mat4::RotationAroundAxis(Vec3(1.0f, 0.0f, 0.0f), m_relativeCameraRotation.GetX());
		Mat4 rotation = y * p;
		Mat4 fullRotation = pSpatial->CalcRotationMatrix() * rotation;
		
		float checkDist = m_distanceMultiplier * (m_positionOffset.Length());
		RayCastingOutput output = CollisionTester::FindWall(m_followTargetPosition, (m_position)-m_followTargetPosition, checkDist);
		
		m_currentDistanceMultiplier = (output.m_didIntersect && output.m_distance < checkDist) ? 0.99f * output.m_distance / (checkDist)* m_distanceMultiplier : m_distanceMultiplier;
		m_currentDistanceMultiplier = MathUtility::Clamp(m_currentDistanceMultiplier, 0.5f, 5.0f);
		
		// calculate desired position
		Vec3 desiredPosition = m_followTargetPosition + ((fullRotation)* (m_positionOffset*m_currentDistanceMultiplier));
		
		// lerp based on springiness
		m_position = (m_position*m_springiness) + (desiredPosition*(1.0f - m_springiness));
		
		// update target
		m_targetPosition = m_followTargetPosition + ((fullRotation)*(m_targetOffset*m_currentDistanceMultiplier));
		
		// mathemagically do things
		m_up = (fullRotation)* Vec3(0.0f, 1.0f, 0.0f);
		
		m_viewDir = m_targetPosition - m_position;
		
		// update world to view
		m_worldToView = Mat4::LookAt(m_position, m_targetPosition, m_up);

		return true;
	}

	Mat4 ChaseCameraComponent::GetWorldToViewMatrix()
	{
		return m_worldToView;
	}

	Mat4 * ChaseCameraComponent::GetWorldToViewMatrixPtr()
	{
		return &m_worldToView;
	}

	Vec3 ChaseCameraComponent::GetUp()
	{
		return m_up;
	}

	Vec3 ChaseCameraComponent::GetViewDir()
	{
		return m_viewDir;
	}

	Vec3 ChaseCameraComponent::GetPosition()
	{
		return m_position;
	}

	Vec3 * ChaseCameraComponent::GetPosPtr()
	{
		return &m_position;
	}

	Vec3 ChaseCameraComponent::GetTargetPosition()
	{
		return m_targetPosition;
	}

	Vec3 ChaseCameraComponent::GetFollowTargetPosition()
	{
		return m_followTargetPosition;
	}

	Vec3 ChaseCameraComponent::GetFollowTargetRotation()
	{
		return m_followTargetRotation;
	}

	Vec3 ChaseCameraComponent::GetPositionOffset()
	{
		return m_positionOffset;
	}

	void ChaseCameraComponent::SetPositionOffset(Vec3 offset)
	{
		m_positionOffset = offset;
	}

	Vec3 ChaseCameraComponent::GetTargetOffset()
	{
		return m_targetOffset;
	}

	void ChaseCameraComponent::SetTargetOffset(Vec3 offset)
	{
		m_targetOffset = offset;
	}

	float ChaseCameraComponent::GetSpringiness()
	{
		return m_springiness;
	}

	float ChaseCameraComponent::GetSpeed()
	{
		return m_moveSpeed;
	}

	float ChaseCameraComponent::GetRotateSpeed()
	{
		return m_rotateSpeed;
	}

	float ChaseCameraComponent::GetDistanceMultiplier()
	{
		return m_distanceMultiplier;
	}

	void ChaseCameraComponent::SetDistanceMultiplier(float value)
	{
		m_distanceMultiplier = value;
	}

	void ChaseCameraComponent::SetSpeed(float newSpeed)
	{
		m_moveSpeed = newSpeed;
	}

	void ChaseCameraComponent::SetRotateSpeed(float speed)
	{
		m_rotateSpeed = speed;
	}

	void ChaseCameraComponent::SetSpringiness(float newSpringiness)
	{
		m_springiness = MathUtility::Clamp(newSpringiness, 0.0f, 1.0f);
	}

	void ChaseCameraComponent::Move(Vec3 newFollowTargetPosition, Vec3 newFollowTargetRotation)
	{
		m_followTargetPosition = newFollowTargetPosition;
		m_followTargetRotation = newFollowTargetRotation;
	}

	void ChaseCameraComponent::Rotate(Vec3 deltaRotation)
	{
		m_relativeCameraRotation = m_relativeCameraRotation + deltaRotation;
	}

	void ChaseCameraComponent::MouseRotate(int dx, int dy)
	{
		m_relativeCameraRotation.GetAddress()[1] -= MathUtility::ToRadians(dx*m_rotateSpeed);
		m_relativeCameraRotation.GetAddress()[0] += MathUtility::ToRadians(dy*m_rotateSpeed);
	}
}
