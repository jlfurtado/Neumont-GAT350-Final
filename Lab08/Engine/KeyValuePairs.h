#ifndef KEYVALUEPAIRS_H
#define KEYVALUEPAIRS_H

// Justin Furtado
// 5/5/2016
// KeyValuePairs.h
// Class that holds a collection of key-value pairs

#include "KeyValuePair.h"
#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED KeyValuePairs
	{
	public:
		// ctor/dtor
		KeyValuePairs();
		~KeyValuePairs();

		// public methods
		bool IsFull() const;
		bool ContainsKey(const char *const key) const;
		bool AddUnique(const char *const key, const char *const value);
		const char *GetValueForKey(const char *const key) const;

	private:
		// private methods
		bool AddKeyValue(const char *const key, const char *const value);

		// private data
		static const int MAX_KEYS = 100;
		static int count;
		static int delcount;
		int m_id;
		int m_numKeys = 0;
		KeyValuePair m_keyValuePairs[MAX_KEYS];

		friend std::ostream& operator<<(std::ostream& os, KeyValuePairs& kvps);
	};
}

#endif // ndef KEYVALUEPAIRS_H