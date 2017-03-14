#include "SoundObject.h"
#include "SoundEngine.h"
#include "GameLogger.h"

SoundObject::SoundObject()
{
}


SoundObject::~SoundObject()
{
	if (m_sound)
		m_sound->drop();
}

bool SoundObject::Initialize(const char* location)
{
	m_location = location;
	return true;
}

void SoundObject::Play(Engine::Vec3 position)
{
	m_sound = SoundEngine::GetEngine()->play3D(m_location, GetKlangPos(position), m_shouldLoop);
}

void SoundObject::PlaySoundLooped(Engine::Vec3 position)
{
	LoopSound();
	Play(position);
}

void SoundObject::Play()
{
	m_sound = SoundEngine::GetEngine()->play2D(m_location, m_shouldLoop);
	m_isPlaying = true;
}

void SoundObject::PlayAsTrack()
{
	m_sound = SoundEngine::GetEngine()->play2D(m_location, m_shouldLoop, false, true);
}

void SoundObject::PlaySoundLooped()
{
	LoopSound();
	Play();
}

void SoundObject::PauseSound() const
{
	if (m_sound)
	{
		m_sound->setIsPaused();
	}
}

void SoundObject::LoopSound(bool loop)
{
	m_shouldLoop = loop;
}

bool SoundObject::GetIsPlaying()
{
	if (m_sound)
		m_isPlaying = !m_sound->isFinished() || !m_sound->getIsPaused();
	return m_isPlaying;
}

bool SoundObject::GetFinished() const
{
	if (m_sound)
	{
		return m_sound->isFinished();
	}
	// m_sound not instantiated yet
	return false;
}

irrklang::vec3df SoundObject::GetKlangPos(Engine::Vec3 position)
{
	return irrklang::vec3df{ position.GetX(), position.GetY(), -position.GetZ()};
}

