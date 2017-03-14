#include "SoundEngine.h"
#include "GameLogger.h"

irrklang::ISoundEngine * SoundEngine::m_soundEngine;

bool SoundEngine::Initialize()
{
	m_soundEngine = irrklang::createIrrKlangDevice();
	if (!m_soundEngine)
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "SoundEngine::Initialize() failed.");
		return false;
	}
	return true;
}

bool SoundEngine::Shutdown()
{
	m_soundEngine->drop();
	return true;
}

