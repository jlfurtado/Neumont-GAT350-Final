#ifndef MOUSECOMPONENT_H
#define MOUSECOMPONENT_H

// Justin Furtado
// 8/16/2016
// MouseComponent.h
// Handles mouse movement

#include "Component.h"
#include "SpatialComponent.h"

class MouseComponent : public Engine::Component
{
public:
	MouseComponent();
	~MouseComponent();

	bool Initialize() override;
	bool Update(float dt) override;
	void MouseMove(int dx, int dy);
	void MouseScroll(int degrees);
	void SetMousePos(int xPos, int yPos);

private:
	bool HandleMouseMovement();
	int lastDX{ 0 };
	int lastDY{ 0 };
	int x{ 0 };
	int y{ 0 };
	float fovy{ 0.0f };
	float nearDist{ 0.0f };
	float width{ 0.0f };
	float height{ 0.0f };
	int degreesScrolled{ 0 };
};

#endif // ifndef MOUSECOMPONENT_H