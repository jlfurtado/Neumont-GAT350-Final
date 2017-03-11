#include "GameLogger.h"
#include "StringFuncs.h"
#include <iomanip>
#include <chrono>
#include <assert.h>
#include <iostream>

// Justin Furtado
// 5/4/2016
// GameLogger.cpp
// Simple logging for games

namespace Engine
{
	int GameLogger::m_counts[(int)MessageType::Num_Types]{ 0 };
	std::ofstream GameLogger::m_logStream;
	bool GameLogger::m_ishtml = false;
	bool GameLogger::isInitialized = false;

	void GameLogger::GetFilePath(const char *const path, const char *const fileName, char *buffer, char *bufferCopy, int bufferSize)
	{
		GetDateTimeStamp(buffer, bufferSize, false);

		StringFuncs::StringCopy(buffer, bufferCopy, bufferSize);

		StringFuncs::StringConcatIntoBuffer(bufferCopy, fileName, ".", buffer, bufferSize);

		StringFuncs::StringCopy(buffer, bufferCopy, bufferSize);

		if (StringFuncs::StringEndsWith(path, "\\") || StringFuncs::StringLen(path) == 0)
		{
			StringFuncs::StringConcatIntoBuffer(path, bufferCopy, "", buffer, bufferSize);
		}
		else
		{
			StringFuncs::StringConcatIntoBuffer(path, bufferCopy, "\\", buffer, bufferSize);
		}
	}

	bool GameLogger::Initialize(const char *const path, const char *const fileName)
	{
		const int buffSize = 512;
		char buffer[buffSize];
		char bufferCopy[buffSize];

		GetFilePath(path, fileName, buffer, bufferCopy, buffSize);

		m_logStream.open(buffer, std::ios::trunc);

		if (!(m_logStream.is_open()))
		{
			// couldn't open file, clean up and return false
			std::cerr << "GameLogger Failed To Initialize!\n";
			return false;
		}

		m_ishtml = StringFuncs::StringEndsWith(buffer, ".html");
		isInitialized = true;

		if (m_ishtml) { WriteHTMLHeader(); }

		GameLogger::Log(MessageType::Info, "Opened file [%s] for logging!\n", buffer);

		// log initialize successful
		GameLogger::Log(MessageType::Process, "GameLogger Initialized successfully!\n");
		return true;
	}

	void GameLogger::WriteLog(MessageType messageType, const char *const message)
	{
		if (!isInitialized) return;

		if (m_logStream)
		{
			if (ConsoleOut(messageType))
			{
				WriteToConsole(messageType, message);
			}

			if (messageType != MessageType::ConsoleOnly)
			{
				const int bufferSize = 30;
				char buffer[bufferSize];
				GetDateTimeStamp(buffer, bufferSize, true);
				if (!m_ishtml) { m_logStream << std::left << std::setw(30) << buffer << " [" << std::setw(16) << GetMessageTypeStr(messageType) << "] : " << message; }
				else
				{
					char messageCopy[MAX_LOG_SIZE];
					// trim \n from html message logs???
					StringFuncs::StringCopy(message, messageCopy, StringFuncs::StringLen(message));
					m_logStream << "\n\t\t<p " << GetMessageStyleStr(messageType) << ">" << std::left << std::setw(30) << buffer << " [" << std::setw(16) << GetMessageTypeStr(messageType) << "] : " << messageCopy << "</p>";
				}
				++m_counts[MsgType(messageType)];
				m_logStream.flush();
			}
		}
		else
		{
			std::cerr << "ERROR: Called WriteLog but file stream was not open!\n";
		}

	}

	void GameLogger::WriteToConsole(MessageType messageType, const char *const message)
	{
		if (messageType != MessageType::ConsoleOnlyNoMessageType)
		{
			printf("[%-16s]: %s", GetMessageTypeStr(messageType), message);
		}
		else
		{
			printf("%s", message);
		}
	}

	bool GameLogger::ShutDown()
	{
		GameLogger::Log(MessageType::Process, "GameLogger Shut Down Successfully!\n");
		if (!m_ishtml) { ShowStats(m_logStream); }
		else { WriteHTMLFooter(); }

		ShowStats(std::cout, true);

		m_logStream.close();
		isInitialized = false;
		return true;
	}

	void GameLogger::ShowStats(std::ostream& os, bool isCout)
	{
		os << (m_ishtml && !isCout ? "<p>" : "") << "Begin Counts For Each Message Type" << (m_ishtml && !isCout ? "</p>" : "\n");

		for (int j = 0; j < (int)MessageType::Num_Types; ++j)
		{
			if (m_counts[j] > 0)
			{
				os << (m_ishtml && !isCout ? "<p>" : "") << "   " << GetMessageTypeStr((MessageType)j) << ": " << m_counts[j] << (m_ishtml && !isCout ? "</p>" : "\n");
			}
		}

		os << (m_ishtml && !isCout ? "<p>" : "") << "End Counts For Each Message Type" << (m_ishtml && !isCout ? "</p>" : "\n");
	}

	char *GameLogger::GetDateTimeStamp(char *const buffer, int bufferSize, bool useColons)
	{
		const char* const format = useColons ? "%F %T." : "%F %H-%M-%S.";

		// get the current time, then its fractional part, then just the milliseconds/microseconds
		auto timeNow = std::chrono::system_clock::now();
		auto nowFractional = timeNow - std::chrono::time_point_cast<std::chrono::seconds>(timeNow);
		//auto millisecs     = std::chrono::duration_cast<std::chrono::milliseconds>(nowFractional).count();
		auto microsecs = std::chrono::duration_cast<std::chrono::microseconds>(nowFractional).count();

		// obtain the structures that allow us to format the time
		time_t    ttNow = std::chrono::system_clock::to_time_t(timeNow);
		struct tm tmNow;
		if (localtime_s(&tmNow, &ttNow)) assert(false); // localtime_s() return 0 on success

														// write the formatted time directly into the output buffer
		size_t numChars = strftime(buffer, bufferSize, format, &tmNow);
		assert(numChars > 0); // strftime() returns 0 on error

							  // concat the milli/micro seconds directly into the buffer
		int len = static_cast<int>(numChars);
		//sprintf_s (buffer + len, bufferSize - len, "%03I64d", millisecs);
		sprintf_s(buffer + len, bufferSize - len, "%06I64d", microsecs);

		return buffer;
	}

	char *GameLogger::GetMessageTypeStr(MessageType messageType)
	{
		char *MessageTypeWords[] = {
			"Unknown Message Type",
			"Console Only",
			"Process",
			"Process",
			"Warning",
			"Warning",
			"Error",
			"Error",
			"Fatal Error",
			"Fatal Error",
			"Info",
			"Info",
			"Debug",
			"Debug",
			"Programmer Error",
			"Programmer Error",
			"No Message Type",
			"No Message Type",
		};

		return MessageTypeWords[static_cast<int>(messageType)];
	}

	char *GameLogger::GetMessageStyleStr(MessageType messageType)
	{
		char *MessageTypeStyles[] = {
			"style=\"color:black;\"",
			"style=\"color:black;\"",
			"style=\"color:blue;\"",
			"style=\"color:blue;\"",
			"style=\"color:gold;\"",
			"style=\"color:gold;\"",
			"style=\"color:red;\"",
			"style=\"color:red;\"",
			"style=\"color:red;\"",
			"style=\"color:red;\"",
			"style=\"color:green;\"",
			"style=\"color:green;\"",
			"style=\"color:purple;\"",
			"style=\"color:purple;\"",
			"style=\"color:red;\"",
			"style=\"color:red;\""
		};

		return MessageTypeStyles[static_cast<int>(messageType)];
	}

	void GameLogger::WriteHTMLHeader()
	{
		m_logStream << std::left << std::setw(30) << "<!DOCTYPE html>\n\t<html>\n\t\t<head>\n\t\t\t<style>body{ background-color: #afafaf; }</style>\n\t\t</head>\n\t<body>";
	}

	void GameLogger::WriteHTMLFooter()
	{
		ShowStats(m_logStream);
		// TODO: Log differing message types in seprate format ,inside different content ? <div></div>? in order to display them differently
		// also, make it look WAAAAYYY cooler, center stuff and things...
		m_logStream << std::left << std::setw(30) << "\n\t</body>\n</html>";
	}

	int GameLogger::GetMessageCount(MessageType messageType)
	{
		return m_counts[MsgType(messageType)];
	}

}