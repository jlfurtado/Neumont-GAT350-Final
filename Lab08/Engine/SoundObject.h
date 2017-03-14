#ifndef SOUND_OBJECT_H_
#define SOUND_OBJECT_H_

// Wesley Sheng
// 3/14/17 (math.pi day!)
// SoundObject.h
// Manages a single sound object

#include <ik_ISound.h>

#include "Vec3.h"

class SoundObject
{
public:
	SoundObject();
	~SoundObject();

	bool Initialize(const char * location);
	void Play(Engine::Vec3 position);
	void PlaySoundLooped(Engine::Vec3 position);
	void Play();
	void PlayAsTrack();
	void PlaySoundLooped();
	void PauseSound() const;
	void LoopSound(bool loop = true);

	bool GetIsPlaying();
	bool GetFinished() const;
private:
	static irrklang::vec3df GetKlangPos(Engine::Vec3 position);


	irrklang::ISound * m_sound { nullptr };
	const char * m_location;
	bool m_isPlaying {false};
	bool m_isPaused;
	bool m_shouldLoop;
};



#endif // ndef SOUND_OBJECT_H_