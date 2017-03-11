#ifndef ENTITY_H
#define ENTITY_H

// Justin Furtado
// Entity.h
// 8/15/2016
// Modified In-Class Code

#include <ostream>
#include "ExportHeader.h"

namespace Engine
{
	class Component;
	class ENGINE_SHARED Entity
	{
		enum
		{
			MAX_COMPONENTS = 10, // increase this when you need more
			MAX_NAME_LEN = 32
		};

	public:
		Entity(const char* name = nullptr);
		~Entity();

		bool        Initialize();
		bool        Shutdown();
		bool        Update(float deltaTime); // calls Update on all Components

		void        SetName(const char* name);
		const char* GetName() const { return m_name; }

		bool        AddComponent(Component* component, const char* componentName);
		std::ostream& Output(std::ostream& os);

		template <class T> T* GetComponentByType() const;

	protected:
		Component* m_components[MAX_COMPONENTS]{ nullptr };
		char       m_name[MAX_NAME_LEN]{ 0 };
	};

	template <class T>
	T* Entity::GetComponentByType() const
	{
		for (int j = 0; j < MAX_COMPONENTS; ++j)
		{
			if (!m_components[j]) continue;
			if (typeid(T) == typeid(*m_components[j]))
			{
				return static_cast<T*>(m_components[j]);
			}
		}
		return nullptr;
	}

	std::ostream& operator<<(std::ostream& os, Entity& entity);
}

#endif // ifndef ENTITY_H