#ifndef SOUNDENGINE_H_
#define SOUNDENGINE_H_

// Wesley Sheng
// SoundEngine.h
// Manages irrKlang's SoundObject Engine

#include "irrKlang.h"

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