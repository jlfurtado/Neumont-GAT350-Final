#include "MessageType.h"

// Justin Furtado
// 5/5/2016
// MessageType.cpp
// Operator overload for enum for logging message types

namespace Engine
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
		"NoMessageType",
		"NoMessageType"
	};

	std::ostream& operator<<(std::ostream& os, MessageType messageType)
	{
		return os << MessageTypeWords[(int)messageType];
	}

	bool ConsoleOut(MessageType messageType)
	{
		// Uses bitwise and
		// 1 & 1 is true
		// 2 & 1 is false
		return (static_cast<int>(messageType) & 1);
	}

	int MsgType(MessageType messageType)
	{
		return static_cast<int>(messageType) - (ConsoleOut(messageType) && (messageType != MessageType::ConsoleOnly) ? 1 : 0);
	}
}