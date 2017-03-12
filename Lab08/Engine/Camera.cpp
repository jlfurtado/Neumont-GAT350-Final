#include "Camera.h"
#include "Camera.h"
#include "GameLogger.h"
#include "MathUtility.h"

// Justin Furtado
// 7/10/2016
// Camera.cpp
// Shows the world!

namespace Engine
{
	Camera::Camera()
		: m_cameraSpeed(1.0f), m_rotateSpeed(1.0f), m_viewDirection(Vec3(0.0f, 0.0f, -1.0f)), m_position(Vec3(175.0f, 20.0f, 200.0f)), m_up(Vec3(0.0f, 1.0f, 0.0f)), m_pitch(0.0f), m_yaw(0.0f), m_following(false)
	{
	}

	Camera::~Camera()
	{
	}

	Mat4 Camera::GetWorldToViewMatrix()
	{
		return Mat4::LookAt(m_position, m_position + m_viewDirection, m_up);
	}

	bool Camera::Initialize()
	{
		GameLogger::Log(MessageType::Process, "Camera initialized successfully!\n");
		return true;
	}

	bool Camera::Shutdown()
	{
		GameLogger::Log(MessageType::Process, "Camera shutdown successfully!\n");
		return true;
	}

	void Camera::StrafeLeft(float dt)
	{
		m_position = m_position - (m_cameraSpeed * dt) * m_viewDirection.Cross(m_up).Normalize();
	}

	void Camera::StrafeRight(float dt)
	{
		m_position = m_position + (m_cameraSpeed * dt) * m_viewDirection.Cross(m_up).Normalize();
	}

	void Camera::Ascend(float dt)
	{
		m_position = m_position + (m_cameraSpeed * dt) * m_up;
	}

	void Camera::Descend(float dt)
	{
		m_position = m_position - (m_cameraSpeed * dt) * m_up;
	}

	void Camera::MoveForward(float dt)
	{
		m_position = m_position + (m_cameraSpeed * dt) * m_viewDirection;
	}

	void Camera::MoveBackward(float dt)
	{
		m_position = m_position - (m_cameraSpeed * dt) * m_viewDirection;
	}

	void Camera::SetSpeed(float unitsPerSecond)
	{
		if (unitsPerSecond < 0.0f) { return; } // Don't allow setting negative speed as it inverts directions
		m_cameraSpeed = unitsPerSecond;
	}

	void Camera::SetRotateSpeed(float speed)
	{
		m_rotateSpeed = speed;
	}

	float Camera::GetRotateSpeed()
	{
		return m_rotateSpeed;
	}

	float Camera::GetSpeed()
	{
		return m_cameraSpeed;
	}

	float Camera::GetYaw()
	{
		return m_yaw;
	}

	float Camera::GetPitch()
	{
		return m_pitch;
	}

	void Camera::MouseRotate(int dx, int dy)
	{
		float deltaY = -dy * m_rotateSpeed;
		float deltaX = -dx * m_rotateSpeed;
		m_pitch += deltaY;
		m_yaw += deltaX;

		if (m_pitch > 89.0f) { m_pitch = 89.0f; return; }
		if (m_pitch < -89.0f) { m_pitch = -89.0f; return; }

		Vec3 right = m_viewDirection.Cross(m_up);
		Mat4 horiz = Mat4::RotationAroundAxis(m_up, MathUtility::ToRadians(deltaX));
		Mat4 vert = Mat4::RotationAroundAxis(right, MathUtility::ToRadians(deltaY));
		m_viewDirection = ((vert * horiz) * m_viewDirection).Normalize();
	}

	Vec3 Camera::GetPosition()
	{
		return m_position;
	}

	void Camera::SetPosition(Vec3 newPosition)
	{
		m_position = newPosition;
	}

	Mat4 Camera::GetRotMat()
	{
		return Engine::Mat4::RotationAroundAxis(Engine::Vec3(0.0f, 0.0, 1.0f).Cross(m_viewDirection), Engine::MathUtility::GetVectorAngleRadians(Engine::Vec3(0.0f, 0.0f, 1.0f), m_viewDirection));
	}

	void Camera::SetViewDirectionDirectly(Vec3 viewDirection)
	{
		m_viewDirection = viewDirection;
	}

	float Camera::WrapAngle(float angleToWrap)
	{
		if (angleToWrap < -180.0f)
		{
			angleToWrap += 360.0f;
		}

		if (angleToWrap > 180.0f)
		{
			angleToWrap -= 360.0f;
		}

		return angleToWrap;
	}
}
