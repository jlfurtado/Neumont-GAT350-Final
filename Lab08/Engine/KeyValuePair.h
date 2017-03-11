#ifndef KEYVALUEPAIR_H
#define KEYVALUEPAIR_H

// Justin Furtado
// 5/5/2016
// KeyValuePair.h
// Simple class to hold keys and values in strings

#include <ostream>
#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED KeyValuePair
	{
	public:
		// ctor/dtor
		KeyValuePair();
		KeyValuePair(const char *const key, const char *const value);
		~KeyValuePair();

		// methods
		void ResetPair(const char *const key, const char *const value);

		// data members
		static const int MAX_CHARS = 256;
		static int count;
		static int delcount;
		char m_key[MAX_CHARS];
		char m_value[MAX_CHARS];
		unsigned int m_id;
	};
}

#endif // ndef KEYVALUEPAIR_H