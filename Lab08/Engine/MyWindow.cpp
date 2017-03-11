
// Order matters
#include "MyWindow.h"

#include "QT\qapplication.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4127)
#include "QtGui\QWheelEvent"
#pragma warning(pop)

#include "GameLogger.h"
#include "GameTime.h"
#include "MouseManager.h"

// Justin Furtado
// 6/21/2016
// MyWindow.cpp
// In-Class Code

namespace Engine
{
	bool isShutdown = false;

	bool MyWindow::Initialize()
	{
		GLenum initResult = glewInit();
		if (initResult != GLEW_OK)
		{
			Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Game Failed to Initialize!!!, Glew error #%d\n", initResult);
			return false;
		}

		this->setFocusPolicy(Qt::StrongFocus);

		if (!Engine::GameTime::Initialize())
		{
			Shutdown();
			Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "MyWindow could not initialize because GameTime failed to initialize!\n");
			return false;
		}

		setMouseTracking(true);
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(Update()));

		if (!m_gameInit(m_pGame, this)) { Shutdown(); return false; } // Exit the window, release resources if initialization fails

		m_timer.start();
		Engine::GameLogger::Log(Engine::MessageType::cProcess, "MyWindow Initialized successfully!!!\n");
		return true;
	}

	bool MyWindow::Shutdown()
	{
		m_pApp->exit();

		isShutdown = true;
		Engine::GameLogger::Log(Engine::MessageType::cProcess, "MyWindow Shutdown successfully!!!\n");
		return true;
	}

	void MyWindow::mouseMoveEvent(QMouseEvent * e)
	{
		static int lastX = 0;
		static int lastY = 0;

		if (m_mouseMove)
		{
			int deltaX = e->x() - lastX;
			int deltaY = e->y() - lastY;

			m_mouseMove(m_pGame, deltaX, deltaY);
		}

		MouseManager::MouseMove(e->x(), e->y());

		lastX = e->x();
		lastY = e->y();
	}

	void MyWindow::mousePressEvent(QMouseEvent * /*e*/)
	{
	}

	void MyWindow::mouseReleaseEvent(QMouseEvent * /*e*/)
	{
	}

	void MyWindow::resizeEvent(QResizeEvent * /*e*/)
	{
		if (m_resize) { m_resize(m_pGame); }
	}

	void MyWindow::wheelEvent(QWheelEvent * e)
	{
		if (m_scroll)
		{
			m_scroll(m_pGame, e->delta() / 8);
		}

		e->accept();
	}

	void MyWindow::Update()
	{
		float dt = Engine::GameTime::GetLastFrameTime();
		if (!isShutdown && m_gameUpdate)
		{ 
			MouseManager::SetMouseState((QApplication::mouseButtons() & Qt::LeftButton) != 0, (QApplication::mouseButtons() & Qt::RightButton) != 0);

			m_gameUpdate(m_pGame, dt);
		}
		if (!isShutdown && m_gameDraw) { m_gameDraw(m_pGame); }
		this->repaint();
	}
}
