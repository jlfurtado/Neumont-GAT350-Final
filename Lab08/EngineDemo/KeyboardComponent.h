#ifndef KEYBOARDCOMPONENT_H
#define KEYBOARDCOMPONENT_H

// Justin Furtado
// 8/16/2016
// KeyboardComponent.h
// Detects key presses and calls the proper component methods

#include "Keyboard.h"
#include "Component.h"
class KeyboardComponent : public Engine::Component
{
public:
	KeyboardComponent();
	~KeyboardComponent();

	bool Initialize() override;
	bool Update(float dt) override;

private:
	bool HandleKeyboardInput(float dt);
	Engine::Keyboard m_keyboardManager;
};

#endif // ifndef KEYBOARDCOMPONENT_H