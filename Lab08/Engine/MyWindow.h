#ifndef MYWINDOW_H
#define MYWINDOW_H

// Justin Furtado
// 6/21/2016
// MyWindow.h
// In-Class Code

#include "GL\glew.h" // Needs to be first

// Note: wasn't giving warnings for me but was for others so I disabled to be extra cautious
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4251)
#pragma warning(disable : 4800)
#include "QtOpenGL\qglwidget"
#pragma warning(pop)

#include "Qt\qapplication.h"

#include "Qt\qtimer.h"
#include "ExportHeader.h"

namespace Engine
{
	class MyWindow;
	typedef bool(*GameInitializeCallback)(void *game, MyWindow *pWindow);
	typedef void(*GameUpdateCallback)(void *game, float dt);
	typedef void(*GameDrawCallback)(void *game);
	typedef void(*WindowResizeCallback)(void *instance);
	typedef void(*MouseScrollCallback)(void *instance, int degrees);
	typedef void(*MouseMoveCallback)(void *instance, int deltaX, int deltaY);

	class ENGINE_SHARED MyWindow : public QGLWidget
	{
		// Needed for QT
		Q_OBJECT

	public:
		// ctor/dtor
		MyWindow(QApplication *app, void *game,
			GameInitializeCallback init, WindowResizeCallback resize,
			MouseMoveCallback mouseMove,
			GameDrawCallback draw, GameUpdateCallback update,
			MouseScrollCallback scroll)
			: m_pApp(app), m_pGame(game), m_gameDraw(draw),
			m_gameUpdate(update), m_gameInit(init), m_resize(resize),
			m_scroll(scroll), m_mouseMove(mouseMove) {}
		~MyWindow() {}

		// methods
		bool Initialize();
		bool Shutdown();

	protected:
		void initializeGL() { Initialize(); }
		void mouseMoveEvent(QMouseEvent *e);
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void resizeEvent(QResizeEvent *e);
		void wheelEvent(QWheelEvent *e);

		private slots:
		void Update();

	private:
		QApplication *m_pApp;
		void *m_pGame;
		QTimer m_timer;
		GameUpdateCallback m_gameUpdate;
		GameDrawCallback m_gameDraw;
		WindowResizeCallback m_resize;
		GameInitializeCallback m_gameInit;
		MouseScrollCallback m_scroll;
		MouseMoveCallback m_mouseMove;
	};
}


#endif // ifndef MYWINDOW_H