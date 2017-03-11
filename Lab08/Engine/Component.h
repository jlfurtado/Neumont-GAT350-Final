#ifndef COMPONENT_H
#define COMPONENT_H

// Justin Furtado
// Component.h
// 8/15/2016
// Modified in-class code

#include "GameLogger.h"
#include "Entity.h"
#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED Component
	{
		enum
		{
			MAX_NAME_LEN = 30
		};

	public:
		Component();
		~Component();

		bool Init();        // called when the Owner Entity is Initialized

		virtual bool Initialize() { return true; }
		virtual bool Update(float /*deltaTime*/) { return true; }

		void        SetName(const char* name);
		const char* GetName() const { return m_name; }
		void        SetOwner(Entity* owner) { m_owner = owner; }
		void        Enable(bool enable = true) { m_enabled = enable; }
		bool        IsEnabled() const { return m_enabled; }
		bool        IsDisabled() const { return !m_enabled; }

		template <class T> T* GetSiblingComponent()
		{
			if (!m_owner) { GameLogger::Log(MessageType::cFatal_Error, "GetSiblingComponent failed! m_owner was nullptr!\n"); }
			return m_owner->GetComponentByType<T>();
		}

	protected:
		Entity* m_owner{ nullptr };             // owner of this component
		char    m_name[MAX_NAME_LEN]{ 0 }; // name  of this component
		bool    m_enabled{ true };
	};
}

#endif // ifndef COMPONENT_H