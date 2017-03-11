// Order matters
#include "EngineDemo.h"
#include "MyWindow.h"

// Note: wasn't giving warnings for me but was for others so I disabled to be extra cautious
#pragma warning(push)
#pragma warning(disable : 4127)
#include "QT\qapplication.h"
#pragma warning(pop)

#include "GameLogger.h"
#include "ConfigReader.h"
#include "MyGL.h"
#include <ctime>

int Run(int argc, char **argv)
{
	srand((unsigned)time(0));
	QApplication app(argc, argv);
	EngineDemo game;
	Engine::MyWindow *window = new Engine::MyWindow(&app, &game, EngineDemo::InitializeCallback, EngineDemo::ResizeCallback, EngineDemo::MouseMoveCallback, EngineDemo::DrawCallback, EngineDemo::UpdateCallback, EngineDemo::MouseScrollCallback);

	bool fullscreen = false;
	Engine::ConfigReader::pReader->GetBoolForKey("EngineDemo.ShowFullscreen", fullscreen);
	if (fullscreen)
	{
		window->showFullScreen();
	}
	else 
	{
		window->show();
	}

	int appResult = app.exec();

	delete window;

	return appResult;
}

const int EXIT_CONFIG_FAIL_INIT = 3;
const int EXIT_CONFIG_FAIL_SHUTDOWN = -3;
int RunWithConfig(int argc, char **argv)
{
	Engine::ConfigReader reader;
	if (!reader.Initialize("..\\Data\\EngineDemo.config")) return EXIT_CONFIG_FAIL_INIT;

	int result = Run(argc, argv);

	if (!reader.ShutDown()) return EXIT_CONFIG_FAIL_SHUTDOWN;

	return result;
}

const int EXIT_LOGGER_FAIL_INIT = 2;
const int EXIT_LOGGER_FAIL_SHUTDOWN = -2;
int RunWithLogger(int argc, char **argv)
{
	if (!Engine::GameLogger::Initialize("..\\Data\\Logs", "EngineDemoGameLog.html")) return EXIT_LOGGER_FAIL_INIT;

	int result = RunWithConfig(argc, argv);

	if (!Engine::GameLogger::ShutDown()) return EXIT_LOGGER_FAIL_SHUTDOWN;

	return result;
}

int main(int argc, char **argv)
{
	int result = RunWithLogger(argc, argv);
	return result;
}