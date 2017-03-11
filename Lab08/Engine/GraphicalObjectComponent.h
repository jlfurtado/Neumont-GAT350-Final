#ifndef GRAPHICALOBJECTCOMPONENT_H
#define GRAPHICALOBJECTCOMPONENT_H

// Justin Furtado
// GraphicalObjectComponent.h
// 8/16/2016
// Holds a graphical object so entities can be drawn

#include "Component.h"
#include "ExportHeader.h"

namespace Engine
{
	class GraphicalObject;

	class ENGINE_SHARED GraphicalObjectComponent : public Component
	{
	public:
		GraphicalObjectComponent();
		~GraphicalObjectComponent();

		bool Initialize() override;
		bool Update(float dt) override;

		void SetGraphicalObject(GraphicalObject *pGraphicalObject);
		GraphicalObject *GetGraphicalObject();

	private:
		GraphicalObject *m_pGraphicalObject;

	};
}

#endif // ifndef GRAPHICALOBJECTCOMPONENT_H