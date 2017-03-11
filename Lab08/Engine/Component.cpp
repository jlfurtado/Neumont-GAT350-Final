#include "Component.h"
#include <string.h>
#include <sstream>
#include "Entity.h"

// Justin Furtado
// Component.cpp
// 8/15/2016
// Modified in-class code

namespace Engine
{
	Component::Component()
	{
	}

	Component::~Component()
	{
	}

	void Component::SetName(const char * name)
	{
		strncpy_s(m_name, MAX_NAME_LEN, name, _TRUNCATE);
	}

	bool Component::Init()
	{
		// make the component name a concatenation of its owner's name and its name
		std::ostringstream buffer;
		buffer << m_owner->GetName() << "." << GetName();

		bool result = Initialize(); // initialize the derived component

									// log the results and return accordingly
		if (result) { GameLogger::Log(MessageType::Process, "Component [%s] initialized successfully!\n", buffer.str().c_str()); }
		else { GameLogger::Log(MessageType::cFatal_Error, "Component [%s] failed to initialize!\n", buffer.str().c_str()); }

		return result;
	}
}