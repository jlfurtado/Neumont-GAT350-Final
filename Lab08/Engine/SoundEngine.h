#ifndef SOUNDENGINE_H_
#define SOUNDENGINE_H_

// Wesley Sheng
// SoundEngine.h
// Manages irrKlang's SoundObject Engine

#pragma warning(push)
#pragma warning(disable: 4251)
#include "irrKlang.h"
#pragma warning(pop)

class SoundEngine
{
public:
	static bool Initialize();
	static bool Shutdown();

	static irrklang::ISoundEngine * GetEngine() { return m_soundEngine; }

private:
	static irrklang::ISoundEngine * m_soundEngine;
};


#endif // ndef SOUNDENGINE_H_