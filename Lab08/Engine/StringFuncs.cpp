#include "StringFuncs.h"

// Justin Furtado
// 5/4/2016
// StringFuncs.h
// Updated c-style string manipulation utilities

namespace Engine
{
	// Counts characters in a string by iterating through them until '\0' is encountered
	int StringFuncs::StringLen(const char *const str)
	{
		if (!str) return 0;
		int pos = 0;

		while (*(str + pos))
		{
			pos++;
		}

		return pos;
	}

	// Compares two strings by first comparing their lengths and, if they are of the same length, comparing each character in the string to the character at the same index in the other string.
	// If any characters are not equal, the strings are not equal
	bool StringFuncs::StringsAreEqual(const char *const str1, const char *const str2)
	{
		if (!str1 || !str2) return str1 == str2;

		if (StringLen(str1) != StringLen(str2)) return false;

		for (int pos = 0; *(str1 + pos) && *(str2 + pos); ++pos)
		{
			if (*(str1 + pos) != *(str2 + pos)) return false;
		}

		return true;
	}

	// Tests if a string is a palindrome by comparing the first half of the characters to the second half of the characters, if they are not equal, it is not a palindrome
	// Ex: madam, m == m && a == a, true
	bool StringFuncs::IsPalindrome(const char *const str)
	{
		if (!str) return true;

		for (int pos = 0, last = StringLen(str) - 1; last - pos > pos; ++pos)
		{
			if (*(str + pos) != *(str + last - pos)) return false;
		}

		return true;
	}

	// Compares each of the characters in each string until either a character that is not equal or the end of a string has been reached.
	// Then it returns + or - 1 depending on which comes first alphabetically
	int StringFuncs::StringCompare(const char *const str1, const char *const str2)
	{
		if (!str1 || !str2) return str1 == str2 ? 0 : str1 < str2 ? 1 : -1;

		int pos;

		for (pos = 0; *(str1 + pos) && *(str2 + pos); ++pos)
		{
			int diff = ((*(str1 + pos)) - (*(str2 + pos)));
			if (diff) return (diff < 0 ? -1 : 1); // if not equal, reurn + or - 1 based on diff
		}

		// end of a string reached
		int diff = ((*(str1 + pos)) - (*(str2 + pos)));
		if (!diff) return 0;
		else return diff < 0 ? -1 : 1;
	}

	// Locates the first occurance of substring target within string source, if not found -1 is returned
	// iterates through source, comparing characters to target. if equal, it compares to the next character in target, else it starts looking again
	// if the end of target is reached when before the end of the source has been reached, a substring has been found that ends at sPos
	int StringFuncs::FindSubString(const char *const source, const char *const target)
	{
		if (StringLen(source) < StringLen(target)) return -1; // definitely not a substring
		if (StringLen(target) == 0) return 0; // empty string is always found

		for (int sPos = 0, tPos = 0; *(source + sPos); ++sPos)
		{
			if (*(source + sPos) == *(target + tPos))
			{
				tPos++;
				if (!(*(target + tPos))) return sPos - tPos + 1;
			}
			else
			{
				tPos = 0;

				if (*(source + sPos) == *(target + tPos))
				{
					tPos++;
					if (!(*(target + tPos))) return sPos - tPos + 1;
				}
			}
		}

		// end of string reached
		return -1;
	}

	// reads the end of a string to determine if it ends with the target string
	bool StringFuncs::StringEndsWith(const char *const source, const char *const target)
	{
		if (!source || !target) return false;
		if (target == source) return true;

		for (int pos = 0, len = StringLen(target), srcLen = StringLen(source); *(target + pos); ++pos)
		{
			if (*(source + srcLen + pos - len) != *(target + pos))
			{
				return false;
			}
		}

		return true;
	}

	// Swaps the first half of the characters in a given string with the last half of the characters in the string
	char *StringFuncs::StringReverse(char *const str)
	{
		if (!str) return nullptr;

		for (int pos = 0, last = StringLen(str) - 1; last - pos > pos; ++pos)
		{
			char t = *(str + last - pos);
			*(str + last - pos) = *(str + pos);
			*(str + pos) = t;
		}

		return str;
	}

	// Converts uppercase characters ('A' - 'Z') to lowercase characters ('a' - 'z'), leaves other characters in string untouched
	char *StringFuncs::ToLower(char *const str)
	{
		if (!str) return nullptr;

		for (int pos = 0; *(str + pos); ++pos)
		{
			if (*(str + pos) >= 'A' && *(str + pos) <= 'Z') *(str + pos) += ('a' - 'A');
		}

		return str;
	}

	// Converts lowercase characters ('a' - 'z') into uppercase ones ('A' - 'Z'), leaves other characters in string untouched
	char *StringFuncs::ToUpper(char *const str)
	{
		if (!str) return nullptr;

		for (int pos = 0; *(str + pos); ++pos)
		{
			if (*(str + pos) >= 'a' && *(str + pos) <= 'z') *(str + pos) -= ('a' - 'A');
		}

		return str;
	}

	// Copies up to a maximum of targetSize - 1 characters into target from source then adds '\0' to the end
	// Returns number of characters copied
	int StringFuncs::StringCopy(const char *const source, char *const target, int targetSize)
	{
		if (!source || !target || targetSize <= 0) return 0;

		int pos, length;
		for (pos = 0, length = StringLen(source); pos < targetSize - 1 && pos < length; ++pos)
		{
			*(target + pos) = *(source + pos);
		}

		*(target + pos) = '\0';

		return pos;
	}

	// coppies up to targetSize - 1 characters from str1, sep and str2 into target in a way that resembles string concatenation
	int StringFuncs::StringConcatIntoBuffer(const char *const str1, const char *const str2, const char *const sep, char *const target, int targetSize)
	{
		if (!str1 || !str2 || !sep || !target || targetSize <= 0) return 0;

		int pos, len1, len2, len3;

		for (pos = 0, len1 = StringLen(str1), len2 = StringLen(str2), len3 = StringLen(sep); pos < targetSize - 1 && pos < (len1 + len2 + len3); ++pos)
		{
			*(target + pos) = ((pos < len1) ? *(str1 + pos) : ((pos < len1 + len3) ? *(sep - len1 + pos) : *(str2 - len1 - len3 + pos)));
		}

		*(target + pos) = '\0';

		return pos;
	}
}