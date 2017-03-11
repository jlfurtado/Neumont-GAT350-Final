#include "MyGL.h"
#include "GameLogger.h"
#include "StringFuncs.h"

// Justin Furtado
// 6/21/2016
// MyGL.h
// Do stuff and things

namespace Engine
{
	const unsigned int NUM_ERRORS = 6;
	unsigned int errorCodes[NUM_ERRORS] = { GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_OPERATION, GL_STACK_OVERFLOW, GL_STACK_UNDERFLOW, GL_OUT_OF_MEMORY };
	char *errorStrings[NUM_ERRORS] = { "OpenGL Error #0x0500 found: Invalid Enum\n",
		"OpenGL Error #0x0501 found: Invalid Value\n",
		"OpenGL Error #0x0502 found: Invalid Operation\n",
		"OpenGL Error #0x0503 found: Stack Overflow\n",
		"OpenGL Error #0x0504 found: Stack Underflow\n",
		"OpenGL Error #0x0505 found: Out of Memory\n" };

	const char *MyGL::GetErrorMsg(GLenum errNum)
	{
		for (int i = 0; i < NUM_ERRORS; ++i)
		{
			if (errNum == errorCodes[i])
			{
				return errorStrings[i];
			}
		}

		return "Unknown GL Error!!!\n";
	}

	bool MyGL::TestForError(MessageType messageType, const char *msg)
	{
		char buffer[GameLogger::MAX_LOG_SIZE];
		int errorCount = 0;
		int errorCode = glGetError();

		while (errorCode != GL_NO_ERROR)
		{
			++errorCount;
			StringFuncs::StringConcatIntoBuffer(msg, GetErrorMsg(errorCode), " ", buffer, GameLogger::MAX_LOG_SIZE);
			GameLogger::Log(messageType, buffer);
			errorCode = glGetError();
		}

		return errorCount > 0;
	}
}