#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

#include <ostream>
#include "ExportHeader.h"

// Justin Furtado
// 5/5/2016
// MessageType.h
// Enum for logging message types

namespace Engine
{
	enum class ENGINE_SHARED MessageType
	{
		UnknownMessageType = 0,
		ConsoleOnly,
		Process,
		cProcess,
		Warning,
		cWarning,
		Error,
		cError,
		Fatal_Error,
		cFatal_Error,
		Info,
		cInfo,
		Debug,
		cDebug,
		Programmer_Error,
		cProgrammer_Error,
		LogOnlyNoMessageType,
		ConsoleOnlyNoMessageType,

		Num_Types // this one last on purpose
	};

	ENGINE_SHARED std::ostream& operator<<(std::ostream& os, MessageType messageType);

	ENGINE_SHARED bool ConsoleOut(MessageType messageType);
	ENGINE_SHARED int MsgType(MessageType messageType);
}

#endif // ndef MESSAGETYPE_H