#include "MouseComponent.h"
#include "MousePicker.h"
#include "ChaseCameraComponent.h"
#include "MathUtility.h"
#include "CollisionTester.h"
#include "GraphicalObject.h"

// Justin Furtado
// 8/16/2016
// MouseComponent.cpp
// Handles mouse movement

MouseComponent::MouseComponent()
{
}

MouseComponent::~MouseComponent()
{
}

bool MouseComponent::Initialize()
{
	Engine::GameLogger::Log(Engine::MessageType::Process, "MouseComponent [%s] initialized successfully!\n", GetName());
	return true;
}

bool MouseComponent::Update(float /*dt*/)
{
	return HandleMouseMovement();
}

void MouseComponent::MouseMove(int dx, int dy)
{
	lastDX = dx;
	lastDY = -dy;
}

void MouseComponent::MouseScroll(int degrees)
{
	degreesScrolled = degrees;
}

void MouseComponent::SetMousePos(int xPos, int yPos)
{
	this->x = xPos;
	this->y = yPos;
}

const float MIN_DISTANCE_MULTIPLIER = 0.5f;
const float MAX_DISTANCE_MULTIPLER = 5.0f;
bool MouseComponent::HandleMouseMovement()
{	
	static float distanceMultiplier = 1.0f;
	static Engine::GraphicalObject *pLast = nullptr;

	// Requires access to camera to rotate it
	Engine::ChaseCameraComponent *pCamera = GetSiblingComponent<Engine::ChaseCameraComponent>();
	if (!pCamera) { return true; }

	distanceMultiplier = Engine::MathUtility::Clamp(distanceMultiplier - degreesScrolled / 360.0f, MIN_DISTANCE_MULTIPLIER, MAX_DISTANCE_MULTIPLER);
	pCamera->SetDistanceMultiplier(distanceMultiplier);
	
	degreesScrolled = 0;

	pCamera->MouseRotate(lastDX, lastDY);
	lastDX = 0;
	lastDY = 0;

	Engine::MousePicker::SetCameraInfo(pCamera->GetPosition(), pCamera->GetViewDir(), pCamera->GetUp());

	Engine::RayCastingOutput output = Engine::CollisionTester::FindFromMousePos(x, y, 10000.0f);

	if (output.m_didIntersect)
	{ 
		pLast = output.m_belongsTo;
	}

	return true;
}