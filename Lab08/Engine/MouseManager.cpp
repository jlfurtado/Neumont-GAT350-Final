#include "MouseManager.h"
#include "GameLogger.h"

// Justin Furtado
// MouseManager.h
// 11/20/2016
// stores position, location and click states for easy access

namespace Engine
{
	int MouseManager::mousePosX = 0;
	int MouseManager::mousePosY = 0;
	bool MouseManager::mouseLeftDownLast = false;
	bool MouseManager::mouseLeftDownCurrent = false;
	bool MouseManager::mouseRightDownLast = false;
	bool MouseManager::mouseRightDownCurrent = false;

	int MouseManager::GetMouseX()
	{
		return mousePosX;
	}

	int MouseManager::GetMouseY()
	{
		return mousePosY;
	}

	void MouseManager::MouseMove(int newX, int newY)
	{
		mousePosX = newX;
		mousePosY = newY;
	}

	void MouseManager::SetMouseState(bool leftDown, bool rightDown)
	{
		mouseLeftDownLast = mouseLeftDownCurrent;
		mouseRightDownLast = mouseRightDownCurrent;
		mouseLeftDownCurrent = leftDown;
		mouseRightDownCurrent = rightDown;
		
	}

	bool MouseManager::IsLeftMouseDown()
	{
		return mouseLeftDownCurrent;
	}

	bool MouseManager::IsLeftMouseUp()
	{
		return !mouseLeftDownCurrent;
	}

	bool MouseManager::IsLeftMouseClicked()
	{
		return mouseLeftDownCurrent && !mouseLeftDownLast;
	}

	bool MouseManager::IsLeftMouseReleased()
	{
		return !mouseLeftDownCurrent && mouseLeftDownLast;
	}

	bool MouseManager::IsRightMouseDown()
	{
		return mouseRightDownCurrent;
	}

	bool MouseManager::IsRightMouseUp()
	{
		return !mouseRightDownCurrent;
	}

	bool MouseManager::IsRightMouseClicked()
	{
		return mouseRightDownCurrent && !mouseRightDownLast;
	}

	bool MouseManager::IsRightMouseReleased()
	{
		return !mouseRightDownCurrent && mouseRightDownLast;
	}
}
