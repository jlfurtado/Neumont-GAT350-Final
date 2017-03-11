#include "KeyValuePair.h"
#include "StringFuncs.h"
#include "GameLogger.h"

// Justin Furtado
// 5/5/2016
// KeyValuePair.cpp
// Implementations for simple class to hold keys and values in strings

namespace Engine
{
	int KeyValuePair::count = 0;
	int KeyValuePair::delcount = 0;

	KeyValuePair::KeyValuePair()
	{
		m_id = ++count;
		GameLogger::Log(MessageType::Info, "KeyValuePair #%d created with Key [DEFAULT_KEY] and Value [DEFAULT_VALUE]\n", m_id);
		StringFuncs::StringCopy("DEFAULT_KEY", m_key, MAX_CHARS);
		StringFuncs::StringCopy("DEFAULT_VALUE", m_value, MAX_CHARS);
	}

	KeyValuePair::KeyValuePair(const char *const key, const char *const value)
	{
		m_id = ++count;
		GameLogger::Log(MessageType::Info, "KeyValuePair #%d created with Key [%s] and Value [%s]\n", m_id, key, value);
		StringFuncs::StringCopy(key, m_key, MAX_CHARS);
		StringFuncs::StringCopy(value, m_value, MAX_CHARS);
	}

	void KeyValuePair::ResetPair(const char *const key, const char *const value)
	{
		GameLogger::Log(MessageType::Info, "KeyValuePair #%d updated with Key [%s] and Value [%s]\n", m_id, key, value);
		StringFuncs::StringCopy(key, m_key, MAX_CHARS);
		StringFuncs::StringCopy(value, m_value, MAX_CHARS);
	}

	KeyValuePair::~KeyValuePair()
	{
		GameLogger::Log(MessageType::Info, "KeyValuePair #%d with Key [%s] and Value [%s] was destroyed, %d remain!\n", m_id, m_key, m_value, (count - (++delcount)));
	}
}