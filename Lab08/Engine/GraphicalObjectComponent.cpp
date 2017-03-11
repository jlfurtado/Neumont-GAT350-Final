#include "GraphicalObjectComponent.h"


namespace Engine
{
	GraphicalObjectComponent::GraphicalObjectComponent()
	{
	}

	GraphicalObjectComponent::~GraphicalObjectComponent()
	{
	}

	bool GraphicalObjectComponent::Initialize()
	{
		GameLogger::Log(MessageType::Process, "GraphicalObjectComponent [%s] initialized successfully!\n", GetName());
		return true;
	}

	bool GraphicalObjectComponent::Update(float /*dt*/)
	{
		// Do nothing
		return true;
	}

	void GraphicalObjectComponent::SetGraphicalObject(GraphicalObject * pGraphicalObject)
	{
		m_pGraphicalObject = pGraphicalObject;
	}

	GraphicalObject * GraphicalObjectComponent::GetGraphicalObject()
	{
		return m_pGraphicalObject;
	}
}


