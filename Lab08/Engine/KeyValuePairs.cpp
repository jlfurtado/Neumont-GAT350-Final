#include "KeyValuePairs.h"
#include "GameLogger.h"
#include "StringFuncs.h"

// Justin Furtado
// 5/5/2016
// KeyValuePairs.cpp
// Implementations for class that holds a collection of key-value pairs

namespace Engine
{
	int KeyValuePairs::count = 0;
	int KeyValuePairs::delcount = 0;

	KeyValuePairs::KeyValuePairs()
	{
		m_id = ++count;
		GameLogger::Log(MessageType::Info, "KeyValuePairs #%d Created with %d empty pairs!\n", m_id, MAX_KEYS);
	}

	KeyValuePairs::~KeyValuePairs()
	{
		GameLogger::Log(MessageType::Info, "KeyValuePairs #%d Destroyed, %d remain!\n", m_id, (count - (++delcount)));
	}

	bool KeyValuePairs::IsFull() const
	{
		return m_numKeys >= MAX_KEYS;
	}

	bool KeyValuePairs::ContainsKey(const char *const key) const
	{
		return GetValueForKey(key) != nullptr;
	}

	bool KeyValuePairs::AddUnique(const char *const key, const char *const value)
	{
		if (ContainsKey(key))
		{
			GameLogger::Log(MessageType::Warning, "Tried to add Key [%s] with Value [%s] but it a key with the same name already exists inside the collection!\n", key, value);
			return false;
		}

		return AddKeyValue(key, value);
	}

	const char *KeyValuePairs::GetValueForKey(const char *const key) const
	{
		for (int j = 0; j < m_numKeys; ++j)
		{
			if (StringFuncs::StringsAreEqual(key, m_keyValuePairs[j].m_key))
			{
				return &m_keyValuePairs[j].m_value[0];
			}
		}

		return nullptr;
	}

	bool KeyValuePairs::AddKeyValue(const char *const key, const char *const value)
	{
		if (IsFull())
		{
			GameLogger::Log(MessageType::Warning, "Tried to add Key [%s] with Value [%s] but collection of size %d is already full!\n", key, value, MAX_KEYS);
			return false;
		}

		m_keyValuePairs[m_numKeys++].ResetPair(key, value);
		return true;
	}
}