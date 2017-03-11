#ifndef GAMELOGGER_H
#define GAMELOGGER_H

// Justin Furtado
// 5/4/2016
// GameLogger.h
// Simple logging for games

#include <fstream>
#include "MessageType.h"
#include "ExportHeader.h"

namespace Engine
{
	class GameLogger
	{
	public:
		static ENGINE_SHARED const int MAX_LOG_SIZE = 3500;
		static ENGINE_SHARED bool Initialize(const char *const path, const char *const fileName);
		static ENGINE_SHARED bool ShutDown();

		template<typename...Args>
		static void Log(MessageType messageType, const char *const format, Args... args);

	private:
		// methods
		static void GetFilePath(const char *const path, const char *const fileName, char *buffer, char *bufferCopy, int bufferSize);
		static ENGINE_SHARED void WriteLog(MessageType messageType, const char *const message);
		static ENGINE_SHARED void WriteToConsole(MessageType messageType, const char *const message);
		static void ShowStats(std::ostream& os, bool cout = false);
		static char *GetDateTimeStamp(char *const buffer, int bufferSize, bool useColons);
		static char *GetMessageTypeStr(MessageType messageType);
		static int GetMessageCount(MessageType messageType);
		static char *GetMessageStyleStr(MessageType messageType);
		static void WriteHTMLHeader();
		static void WriteHTMLFooter();

		// data 
		static bool isInitialized;
		static bool m_ishtml;
		static int m_counts[static_cast<int>(MessageType::Num_Types)];
		static std::ofstream m_logStream;
	};

	template <typename...Args>
	inline void GameLogger::Log(MessageType messageType, const char *const format, Args...args)
	{
		char buffer[MAX_LOG_SIZE]{ 0 };
		sprintf_s(buffer, MAX_LOG_SIZE, format, args...);
		WriteLog(messageType, buffer);
	}
}

#endif // ndef GAMELOGGER_H