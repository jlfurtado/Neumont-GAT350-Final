#include "Entity.h"
#include <string.h>
#include "Component.h"
#include <iostream>

// Justin Furtado
// Entity.cpp
// 8/15/2016
// Modified In-Class Code

namespace Engine
{
	Entity::Entity(const char* name)
		: m_components{ nullptr }
	{
		if (name) SetName(name);
	}

	Entity::~Entity()
	{
	}

	//
	// Initialize all Components
	//
	bool Entity::Initialize()
	{
		for (int j = 0; j < MAX_COMPONENTS; ++j)
		{
			if (m_components[j])
			{
				// initialize component and, if it fails, log error
				if (!m_components[j]->Init())
				{
					GameLogger::Log(MessageType::cFatal_Error, "Failed to initialize entity [%s]! Failed to initialize component [%s]!\n", GetName(), m_components[j]->GetName());
					return false;
				}
			}
		}

		GameLogger::Log(MessageType::Process, "Initialize was successful for entity [%s]!\n", GetName());
		return true;
	}

	bool Entity::Shutdown()
	{
		GameLogger::Log(MessageType::Process, "Shutdown was successful for Entity [%s]!\n", GetName());
		return true;
	}

	void Entity::SetName(const char * name)
	{
		strncpy_s(m_name, MAX_NAME_LEN, name, _TRUNCATE);
	}

	//
	// Warning! This method should make sure no such component already exists in this entity!
	// Programmer Error if it does!
	//
	bool Entity::AddComponent(Component * component, const char * componentName)
	{
		// validate input
		if (!component) { GameLogger::Log(MessageType::cFatal_Error, "Failed to AddComponent to entity [%s]! Component was nullptr!\n", GetName()); return false; }
		if (!componentName) { GameLogger::Log(MessageType::cFatal_Error, "Failed to AddComponent to entity [%s]! Component Name was nullptr!\n", GetName()); return false; }

		// loop through components and try to find space to add a component
		for (int j = 0; j < MAX_COMPONENTS; ++j)
		{
			if (!m_components[j])
			{
				// if space is found add the component
				m_components[j] = component;
				component->SetOwner(this);
				component->SetName(componentName);
				return true;
			}
		}

		// no space was found, log error
		GameLogger::Log(MessageType::cFatal_Error, "Could not add component [%s] to entity [%s]! No room remains! MAX_COMPONENTS = %d\n", componentName, GetName(), MAX_COMPONENTS);
		return false;
	}

	bool Entity::Update(float deltaTime)
	{
		// loop through components and collect results of their updates
		bool result = true;
		for (int j = 0; j < MAX_COMPONENTS; ++j)
		{
			if (m_components[j] && m_components[j]->IsEnabled())
			{
				result &= m_components[j]->Update(deltaTime);
			}
		}

		return result;
	}

	// This is just to help debug entities
	std::ostream & Entity::Output(std::ostream & stream)
	{
		stream << "===== Begin Entity : (" << GetName() << ") =====\n";
		const int bufferSize = 100;
		char buffer[bufferSize];
		int counter = 0;
		for (int j = 0; j < MAX_COMPONENTS; ++j)
		{
			if (m_components[j])
			{
				sprintf_s(buffer, bufferSize, "   %d : Component (%s)\n",
					counter, m_components[j]->GetName());
				stream << buffer;
				++counter;
			}
		}
		stream << "=====  Done Entity : (" << GetName() << ") =====\n";
		return stream;
	}

	std::ostream & operator<<(std::ostream& os, Entity & entity)
	{
		return entity.Output(os);
	}
}