#include "ChaseCamera.h"
#include "MathUtility.h"

// Justin Furtado
// 7/29/2016
// ChaseCamera.h
// A camera that follows a target

namespace Engine
{
	ChaseCamera::ChaseCamera()
		: m_springiness(0.15f), m_rotateSpeed(1.0f)
	{
	}

	ChaseCamera::ChaseCamera(Vec3 positionOffset, Vec3 targetOffset, Vec3 relativeCameraRotation)
		: m_positionOffset(positionOffset), m_targetOffset(targetOffset), m_relativeCameraRotation(relativeCameraRotation), m_springiness(0.15f), m_rotateSpeed(1.0f)
	{
	}


	ChaseCamera::~ChaseCamera()
	{
	}

	Vec3 ChaseCamera::GetPosition()
	{
		return m_position;
	}

	Vec3 ChaseCamera::GetTargetPosition()
	{
		return m_targetPosition;
	}

	Vec3 ChaseCamera::GetFollowTargetPosition()
	{
		return m_followTargetPosition;
	}

	Vec3 ChaseCamera::GetFollowTargetRotation()
	{
		return m_followTargetRotation;
	}

	Vec3 ChaseCamera::GetPositionOffset()
	{
		return m_positionOffset;
	}

	Mat4 ChaseCamera::GetWorldToView()
	{
		return m_worldToView;
	}

	void ChaseCamera::SetPositionOffset(Vec3 offset)
	{
		m_positionOffset = offset;
	}

	Vec3 ChaseCamera::GetTargetOffset()
	{
		return m_targetOffset;
	}

	void ChaseCamera::SetTargetOffset(Vec3 offset)
	{
		m_targetOffset = offset;
	}

	float ChaseCamera::GetSpringiness()
	{
		return m_springiness;
	}

	float ChaseCamera::GetRotateSpeed()
	{
		return m_rotateSpeed;
	}

	void ChaseCamera::SetRotateSpeed(float speed)
	{
		m_rotateSpeed = speed;
	}

	void ChaseCamera::SetSpringiness(float newSpringiness)
	{
		m_springiness = newSpringiness;
		m_springiness = ((m_springiness < 0.0f) ? 0.0f : m_springiness);
		m_springiness = ((m_springiness > 1.0f) ? 1.0f : m_springiness);
	}

	void ChaseCamera::Move(Vec3 newFollowTargetPosition, Vec3 newFollowTargetRotation)
	{
		m_followTargetPosition = newFollowTargetPosition;
		m_followTargetRotation = newFollowTargetRotation;
	}

	void ChaseCamera::Rotate(Vec3 deltaRotation)
	{
		m_relativeCameraRotation = m_relativeCameraRotation + deltaRotation;
	}

	void ChaseCamera::Update(float /*dt*/)
	{
		// sum rotations so the camera rotation is relative to the model rotation
		Vec3 combinedRotation = m_followTargetRotation + m_relativeCameraRotation;

		// calc rotation matrix for camera
		Mat4 y = Mat4::RotationAroundAxis(Vec3(0.0f, 1.0f, 0.0f), combinedRotation.GetY());
		Mat4 p = Mat4::RotationAroundAxis(Vec3(1.0f, 0.0f, 0.0f), combinedRotation.GetX());
		Mat4 r = Mat4::RotationAroundAxis(Vec3(0.0f, 0.0f, 1.0f), combinedRotation.GetZ());
		Mat4 rotation = y * p * r;

		// calculate desired position
		Vec3 desiredPosition = m_followTargetPosition + (rotation * m_positionOffset);

		// lerp based on springiness
		m_position = (m_position*m_springiness) + (desiredPosition*(1.0f - m_springiness));

		// update target
		m_targetPosition = m_followTargetPosition + (rotation*m_targetOffset);

		// todo figure this out
		Vec3 up = rotation * Vec3(0.0f, 1.0f, 0.0f);

		// update world to view
		m_worldToView = Mat4::LookAt(m_position, m_targetPosition, up);
	}

	void ChaseCamera::MouseRotate(int dx, int dy)
	{
		m_relativeCameraRotation.GetAddress()[1] -= MathUtility::ToRadians(dx*m_rotateSpeed);
		m_relativeCameraRotation.GetAddress()[0] += MathUtility::ToRadians(dy*m_rotateSpeed);

	}
}