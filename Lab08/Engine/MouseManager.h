#ifndef MOUSEMANGER_H
#define MOUSEMANAGER_H

// Justin Furtado
// MouseManager.h
// 11/20/2016
// stores position, location and click states for easy access

#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED MouseManager
	{
	public:
		static bool IsLeftMouseDown();
		static bool IsLeftMouseUp();
		static bool IsLeftMouseClicked();
		static bool IsLeftMouseReleased();
		static bool IsRightMouseDown();
		static bool IsRightMouseUp();
		static bool IsRightMouseClicked();
		static bool IsRightMouseReleased();
		static int GetMouseX();
		static int GetMouseY();

		static void MouseMove(int newX, int newY);
		static void SetMouseState(bool leftDown, bool rightDown);

	private:
		static bool mouseLeftDownLast;
		static bool mouseLeftDownCurrent;
		static bool mouseRightDownLast;
		static bool mouseRightDownCurrent;
		static int mousePosX;
		static int mousePosY;

	};
}

#endif // infdef MOUSEMANAGER_H