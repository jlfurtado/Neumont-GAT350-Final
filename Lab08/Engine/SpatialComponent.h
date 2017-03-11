#ifndef SPATIALCOMPONENT_H
#define SPATIALCOMPONENT_H

// Justin Furtado
// SpatialComponent.h
// 8/15/2016
// Holds transform information for an entity

#include "Component.h"
#include "ExportHeader.h"
#include "Vec3.h"
#include "Mat4.h"

namespace Engine
{
	class ENGINE_SHARED SpatialComponent : public Component
	{
	public:
		SpatialComponent();
		~SpatialComponent();

		bool Initialize() override;
		bool Update(float dt) override;

		void SetPosition(Vec3 newPosition);
		void Translate(Vec3 deltaPosition);
		void SetYaw(float newYaw);
		void Yaw(float amountToYaw);
		void SetPitch(float newPitch);
		void Pitch(float amountToPitch);
		void SetRoll(float newRoll);
		void Roll(float amountToRoll);
		Vec3 GetPosition();
		float GetYaw();
		float GetPitch();
		float GetRoll();
		Vec3 GetForward();
		Vec3 GetUp();
		Vec3 GetRight();
		Mat4 CalcRotationMatrix();
		Vec3 GetVelocity();
		void SetVelocity(Vec3 newVelocity);

	private:
		Vec3 m_position;
		float m_yaw;
		float m_pitch;
		float m_roll;
		Vec3 m_forward;
		Vec3 m_right;
		Vec3 m_up;
		Vec3 m_velocity;
	};
}

#endif // ifndef SPATIALCOMPONENT_H