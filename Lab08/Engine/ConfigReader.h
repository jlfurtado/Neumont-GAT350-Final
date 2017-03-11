#ifndef CONFIGREADER_H
#define CONFIGREADER_H

// Justin Furtado
// 5/5/2016
// ConfigReader.h
// Class that reads in a configuration file and stores the key-value pairs it finds there

#include "ExportHeader.h"
#include "KeyValuePairs.h"
#include <fstream>

namespace Engine
{
	ENGINE_SHARED typedef void(*ConfigReaderCallback)(void *classInstance);
	class ConfigReader
	{
	public:
		// ctor/dtor
		ENGINE_SHARED ConfigReader();
		ENGINE_SHARED ~ConfigReader();

		// public methods
		ENGINE_SHARED bool Initialize(const char *const configFileName);
		ENGINE_SHARED bool ProcessConfigFile(const char *const configFileName);
		ENGINE_SHARED bool ProcessConfigFile();
		ENGINE_SHARED bool ShutDown();

		ENGINE_SHARED bool IsFull() const;
		ENGINE_SHARED bool ContainsKey(const char *const key) const;
		ENGINE_SHARED bool ContainsRequiredKey(const char *const key) const;
		ENGINE_SHARED bool AddUnique(const char *const key, const char *const value);
		ENGINE_SHARED bool GetStringForKey(const char *const key, char* const& outValue) const;
		ENGINE_SHARED bool GetIntForKey(const char *const key, int& outValue) const;
		ENGINE_SHARED bool GetFloatForKey(const char *const key, float& outValue) const;
		ENGINE_SHARED bool GetBoolForKey(const char *const key, bool& outValue) const;
		ENGINE_SHARED bool GetClampedIntForKey(const char *const key, int& outValue, int minValue, int maxValue) const;
		ENGINE_SHARED bool GetClampedFloatForKey(const char *const key, float& outValue, float minValue, float maxValue) const;
		ENGINE_SHARED bool GetFloatsForKey(const char *const key, int numFloats, float *values) const;
		ENGINE_SHARED bool GetIntsForKey(const char *const key, int numInts, int *values) const;
		ENGINE_SHARED bool GetClampedFloatsForKey(const char *const key, int numFloats, float *values, float minValue, float maxValue) const;
		ENGINE_SHARED bool GetClampedIntsForKey(const char *const key, int numInts, int *values, int minValue, int maxValue) const;
		ENGINE_SHARED bool WhiteSpace(char c) const;
		ENGINE_SHARED bool RegisterCallbackForConfigChanges(ConfigReaderCallback callbackToRegister, void *callbackClassInstance);
		ENGINE_SHARED bool UnregisterCallbackForConfigChanges(ConfigReaderCallback callbackToUnregister);
	 
		ENGINE_SHARED static ConfigReader *pReader;

	private:
		// private methods
		bool OpenFile();
		bool ReadFile();
		bool ReadLine(const char *const line);
		bool ReadWord(const char *const line, int wordNum, char *const buffer, int bufferSize);
		void CallCallbacks();

		bool GetIntFromString(const char *const string, int& outValue) const;
		bool GetFloatFromString(const char *const string, float& outValue) const;

		// private data
		static const int MAX_CALLBACKS = 25;
		static int count;
		static int delcount;
		const char *fileName;
		int m_id;
		KeyValuePairs m_keyValuePairs;
		std::ifstream m_configStream;
		ConfigReaderCallback m_callbacks[MAX_CALLBACKS]{ nullptr };
		void *m_pCallbackInstances[MAX_CALLBACKS]{ nullptr };

	};
}

#endif // ndef CONFIGREADER_H