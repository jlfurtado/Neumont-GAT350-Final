#ifndef MYGL_H
#define MYGL_H

// Justin Furtado
// 6/21/2016
// MyGL.h
// Do stuff and things

#include "GL\glew.h"
#include "GameLogger.h"
#include "MessageType.h"
#include "ExportHeader.h"

namespace Engine
{
	class MyGL
	{
	public:
		static ENGINE_SHARED const char* GetErrorMsg(GLenum errNum);
		static ENGINE_SHARED bool        TestForError(MessageType messageType, const char* msg);

		template<typename...Args>
		static bool TestForError(MessageType messageType, const char *const format, Args ...args);
	};

	template<typename ...Args>
	inline bool MyGL::TestForError(MessageType messageType, const char *const format, Args ...args)
	{
		char      buffer[GameLogger::MAX_LOG_SIZE];
		sprintf_s(buffer, GameLogger::MAX_LOG_SIZE, format, args...);
		return TestForError(messageType, buffer);
	}

}

#endif // ifndef MYGL_H