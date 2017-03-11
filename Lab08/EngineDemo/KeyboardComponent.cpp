#include "KeyboardComponent.h"
#include "SpatialComponent.h"
#include "ChaseCameraComponent.h"
#include "GraphicalObjectComponent.h"
#include "GraphicalObject.h"

// Justin Furtado
// 8/16/2016
// KeyboardComponent.h
// Detects key presses and calls the proper component methods

KeyboardComponent::KeyboardComponent()
{
}


KeyboardComponent::~KeyboardComponent()
{
}

bool KeyboardComponent::Initialize()
{
	if (!m_keyboardManager.AddKeys("QWEASD F"))
	{
		Engine::GameLogger::Log(Engine::MessageType::cError, "KeyboardComponent failed to initialize! Could not add keys!\n");
		return false;
	}

	Engine::GameLogger::Log(Engine::MessageType::Process, "KeyboardComponent [%s] initialized successfully!\n", GetName());
	return true;
}

bool KeyboardComponent::Update(float dt)
{
	m_keyboardManager.Update(dt);
	return HandleKeyboardInput(dt);
}

bool KeyboardComponent::HandleKeyboardInput(float dt)
{
	// Requires access to transform info to modify stuff
	Engine::SpatialComponent *pSpatialComponent = GetSiblingComponent<Engine::SpatialComponent>();
	if (!pSpatialComponent) { return true; }

	// Requires access to camera to rotate it
	Engine::ChaseCameraComponent *pCameraComponent = GetSiblingComponent<Engine::ChaseCameraComponent>();
	if (!pCameraComponent) { return true; }

	// Requires access to graphical object to rotate it
	Engine::GraphicalObjectComponent *pGraphicalObjectComponent = GetSiblingComponent<Engine::GraphicalObjectComponent>();
	if (!pGraphicalObjectComponent) { return true; }

	// handle dargon input
	static Engine::Vec3 combinedRotation(0.0f, 0.0f, 0.0f);
	Engine::Vec3 deltaRotation(0.0f, 0.0f, 0.0f);
	if (m_keyboardManager.KeyIsDown('W')) { deltaRotation = deltaRotation + Engine::Vec3(-1.0f, 0.0f, 0.0f);}
	if (m_keyboardManager.KeyIsDown('S')) { deltaRotation = deltaRotation + Engine::Vec3(1.0f, 0.0f, 0.0f); }
	if (m_keyboardManager.KeyIsDown('A')) { deltaRotation = deltaRotation + Engine::Vec3(0.0f, 1.0f, 0.0f); }
	if (m_keyboardManager.KeyIsDown('D')) { deltaRotation = deltaRotation + Engine::Vec3(0.0f, -1.0f, 0.0f); }
	if (m_keyboardManager.KeyIsDown('Q')) { deltaRotation = deltaRotation + Engine::Vec3(0.0f, 0.0f, -1.0f); }
	if (m_keyboardManager.KeyIsDown('E')) { deltaRotation = deltaRotation + Engine::Vec3(0.0f, 0.0f, 1.0f); }

	if (deltaRotation.Length() > 0.0f) { combinedRotation = combinedRotation + deltaRotation.Normalize() * pCameraComponent->GetRotateSpeed() * dt; }
	if (m_keyboardManager.KeyIsDown(' ')) { pSpatialComponent->SetVelocity(pCameraComponent->GetSpeed()*(pSpatialComponent->CalcRotationMatrix()*(Engine::Vec3(0.0f, 0.0f, -1.0f)))); }
	else { pSpatialComponent->SetVelocity(Engine::Vec3(0.0f, 0.0f, 0.0f)); }

	// set rotation
	pSpatialComponent->SetRoll(combinedRotation.GetZ());
	pSpatialComponent->SetPitch(combinedRotation.GetX());
	pSpatialComponent->SetYaw(combinedRotation.GetY());

	// calculate matrix and sqqqqet it on gob so its drawn
	Engine::Mat4 rotation = pSpatialComponent->CalcRotationMatrix();
	pGraphicalObjectComponent->GetGraphicalObject()->SetRotMat(rotation);

	// update camera
	pCameraComponent->Move(pSpatialComponent->GetPosition(), combinedRotation);
	return true;
}
