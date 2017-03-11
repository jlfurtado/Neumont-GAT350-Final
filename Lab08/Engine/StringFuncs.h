#ifndef STRINGFUNCS_H
#define STRINGFUNCS_H

#include "ExportHeader.h"

// Justin Furtado
// 5/4/2016
// StringFuncs.h
// Updated c-style string manipulation utilities

namespace Engine
{
	class ENGINE_SHARED StringFuncs
	{
	public:
		// methods that test/measure/compare strings
		static bool StringsAreEqual(const char *const str1, const char *const str2);
		static bool IsPalindrome(const char *const str);
		static int StringCompare(const char *const str1, const char *const str2);
		static int StringLen(const char *const str);
		static int FindSubString(const char *const source, const char *const target);
		static bool StringEndsWith(const char *const source, const char *const target);

		// methods that modify strings
		static char *StringReverse(char *const str);
		static char *ToLower(char *const str);
		static char *ToUpper(char *const str);
		static int StringCopy(const char *const source, char *const target, int targetSize);
		static int StringConcatIntoBuffer(const char *const str1, const char *const str2, const char *const sep, char *const target, int targetSize);
	};
}

#endif // ndef STRINGFUNCS_H