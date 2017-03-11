#include "BufferManager.h"
#include "GraphicalObject.h"
#include "GameLogger.h"
#include "MyGL.h"
#include "ConfigReader.h"

// Justin Furtado
// 7/14/2016
// BufferManager.cpp
// Manages OpenGL buffers

namespace Engine
{
	BufferGroup BufferManager::s_bufferGroups[MAX_BUFFER_GROUPS];
	GLint BufferManager::s_nextBufferGroup = 0;
	GLuint BufferManager::s_indexCount = 0;
	GLuint BufferManager::s_vertexCount = 0;
	GLuint BufferManager::s_meshCount = 0;
	GLuint BufferManager::s_graphicalObjectCount = 0;
	GLint BufferManager::s_bufferSize = DEFAULT_BUFFER_SIZE;

	bool BufferManager::Initialize()
	{
		// read in config values
		if (!ReadConfigValues())
		{
			GameLogger::Log(MessageType::cWarning, "Failed to read some config values for BufferManager, BufferManager will continue with default values!\n");
		}

		// indicate success
		GameLogger::Log(MessageType::Process, "BufferManager initialized successfully!\n");
		return true;
	}

	bool BufferManager::Shutdown()
	{
		// indicate success
		GameLogger::Log(MessageType::Process, "BufferManager shutdown successfully!\n");
		return true;
	}

	bool BufferManager::AddMesh(Mesh * pMeshToAdd)
	{
		// validate input pointer
		if (!pMeshToAdd)
		{
			GameLogger::Log(MessageType::cError, "BufferManager AddMesh was called with pMeshToAdd as nullptr!\n");
			return false;
		}

		// check if the mesh should go in any existing buffer groups
		bool foundGroup = false;
		for (int i = 0; i < s_nextBufferGroup; ++i)
		{
			if (s_bufferGroups[i].BelongsInThisGroup(pMeshToAdd))
			{
				// The mesh has the same shader and vertex format, it should be added to this group
				if (!s_bufferGroups[i].AddMesh(pMeshToAdd))
				{
					GameLogger::Log(MessageType::cWarning, "Buffer group index [%d] failed to add mesh!\n", i);
					return false;
				}

				foundGroup = true;
				break;
			}
		}

		// if the mesh did not belong in any of the existing buffer groups, create a new one for this mesh and add it
		if (!foundGroup)
		{
			if (s_nextBufferGroup >= MAX_BUFFER_GROUPS)
			{
				GameLogger::Log(MessageType::cWarning, "Buffer manager could not create new buffer group for mesh to be added! Limit of [%d] has bee nreached!\n", MAX_BUFFER_GROUPS);
				return false;
			}

			s_bufferGroups[s_nextBufferGroup].Initialize(s_bufferSize, pMeshToAdd->GetVertexFormat(), pMeshToAdd->GetShaderProgramID(), pMeshToAdd->IsCullingEnabledForObject());

			if (!s_bufferGroups[s_nextBufferGroup++].AddMesh(pMeshToAdd))
			{
				GameLogger::Log(MessageType::cWarning, "Failed to add mesh to brand new buffer group index [%d]!\n", s_nextBufferGroup - 1);
				return false;
			}
		}

		// update statistics to be logged
		s_indexCount += pMeshToAdd->GetIndexCount();
		s_vertexCount += pMeshToAdd->GetVertexCount();
		s_meshCount++;

		// Success
		GameLogger::Log(MessageType::Process, "Buffer Manager Successfully added mesh with [%d] vertices and [%d] indices!\n", pMeshToAdd->GetVertexCount(), pMeshToAdd->GetIndexCount());
		return true;
	}

	bool BufferManager::AddGraphicalObject(GraphicalObject * pGraphicalObjectToAdd)
	{
		// validate input
		if (!pGraphicalObjectToAdd)
		{
			GameLogger::Log(MessageType::cError, "BufferManager AddGraphicalObject was called with pGraphicalObjectToAdd equal to nullptr!\n");
			return false;
		}

		// loop through buffer groups, if it belongs in the group add it to the group
		for (int i = 0; i < s_nextBufferGroup; ++i)
		{
			if (s_bufferGroups[i].BelongsInThisGroup(pGraphicalObjectToAdd))
			{
				if (!s_bufferGroups[i].AddGraphicalObject(pGraphicalObjectToAdd))
				{
					GameLogger::Log(MessageType::cWarning, "Failed to add graphical object to buffer group index [%d]!\n", i);
				}

				// no need to continue processing
				break;
			}
		}

		// Update statistics
		s_graphicalObjectCount++;

		// success
		GameLogger::Log(MessageType::Process, "BufferManager successfully added GraphicalObject located at [%p]!\n", pGraphicalObjectToAdd);
		return true;
	}

	void BufferManager::RemoveGraphicalObject(GraphicalObject * pGraphicalObjectToRemove)
	{
		for (int i = 0; i < s_nextBufferGroup; ++i)
		{
			if (s_bufferGroups[i].BelongsInThisGroup(pGraphicalObjectToRemove))
			{
				s_bufferGroups[i].RemoveGraphicalObject(pGraphicalObjectToRemove);
			}
		}

		// TODO: Update statistics on remove
	}

	void BufferManager::ConsoleLogStats()
	{
		GameLogger::Log(MessageType::ConsoleOnly, "Across all buffers, Vertex Count is [%d], Index Count is [%d], Mesh Count is [%d], Graphical Object Count is [%d]\n", s_vertexCount, s_indexCount, s_meshCount, s_graphicalObjectCount);
	}

	GLint BufferManager::GetNextBufferGroup()
	{
		return s_nextBufferGroup;
	}

	BufferGroup * BufferManager::GetBufferGroups()
	{
		return &s_bufferGroups[0];
	}

	bool BufferManager::ReadConfigValues()
	{
		if (!ConfigReader::pReader->GetClampedIntForKey("EngineDemo.BufferSize", s_bufferSize, MIN_BUFFER_SIZE, MAX_BUFFER_SIZE))
		{
			GameLogger::Log(MessageType::cWarning, "Unable to get clamped int for key [EngineDemo.BufferSize], will use default value!\n");
			return false;
		}

		return true;
	}
}

