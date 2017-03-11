#include "MyFiles.h"

#include <iostream>
#include <fstream>

#include "GameLogger.h"

// Justin Furtado
// 6/26/2016
// MyFiles.cpp
// A utility class for file input

namespace Engine
{
	char *MyFiles::ReadFileIntoString(const char *const fileName)
	{
		std::ifstream inputFileStream(fileName, std::ifstream::binary);

		if (inputFileStream.good())
		{
			std::filebuf *pBuf = inputFileStream.rdbuf();

			// Seek for file length
			auto size = pBuf->pubseekoff(0, inputFileStream.end);
			long bufferSize = 1 + static_cast<long>(size);

			if (bufferSize <= 0)
			{
				GameLogger::Log(MessageType::Error, "Unable to seek to end of file [%s]\n", fileName);
				return nullptr;
			}

			GameLogger::Log(MessageType::Debug, "File [%s] loaded into buffer of size [%ld]\n", fileName, bufferSize);

			char *buffer = new char[bufferSize] {0}; // Note: it is expected that the calling function will delete the buffer returned

													 // Seek to beginning for reading
			pBuf->pubseekpos(0);

			// read in file
			pBuf->sgetn(buffer, size);
			buffer[bufferSize - 1] = '\0'; // null-terminate string

			inputFileStream.close();

			// Don't break because logger buffer too small
			if (bufferSize > (GameLogger::MAX_LOG_SIZE - 50)) // minus fifty for message itself
			{
				GameLogger::Log(MessageType::Error, "MAX_LOG_SIZE of [%d] too small to display contents of file buffer of size [%d]. Log message will not be displayed!!!\n", GameLogger::MAX_LOG_SIZE, bufferSize);
			}
			else
			{
				GameLogger::Log(MessageType::Debug, "Contents of file [%s] are [%s]\n", fileName, buffer);
			}

			return buffer;
		}

		GameLogger::Log(MessageType::Error, "Unable to open file [%s], ifstream is no good!\n", fileName);
		return nullptr;
	}
}
