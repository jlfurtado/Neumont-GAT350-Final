#include <iostream>
#include "ObjConverter.h"
#include "GameLogger.h"
#include "ConfigReader.h"
#include <ctime>

const int EXIT_CONVERTER_FAIL_INIT = 4;
const int EXIT_CONVERTER_FAIL_SHUTDOWN = -4;
int Run(int /*argc*/, char ** /*argv*/)
{
	srand((unsigned)time(0));

	ObjConverter converter;
	if (!converter.Initialize()) return EXIT_CONVERTER_FAIL_INIT;

	bool success = converter.ProcessFile();

	if (!converter.Shutdown()) return EXIT_CONVERTER_FAIL_SHUTDOWN;
	
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
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