#include "BufferGroup.h"
#include "BufferManager.h"
#include "GraphicalObject.h"
#include "ShaderProgram.h"
#include "MyGL.h"

// Justin Furtado
// 7/31/2016
// BufferGroup.cpp
// Holds BufferInfo of similar state in a group

namespace Engine
{
	BufferGroup::BufferGroup()
		: m_nextBufferPair(0), m_shaderProgramID(0), m_vertexFormat(VertexFormat::None)
	{
	}

	BufferGroup::~BufferGroup()
	{
	}

	bool BufferGroup::Initialize(GLuint defaultBufferSize, VertexFormat bufferGroupVertexFormat, GLint shaderProgramID, bool cullForObjectsInBuffer)
	{
		m_defaultBuffersize = defaultBufferSize;
		m_vertexFormat = bufferGroupVertexFormat;
		m_shaderProgramID = shaderProgramID;
		m_cullForObjectsInBuffer= cullForObjectsInBuffer;

		GameLogger::Log(MessageType::Process, "BufferGroup initialize successful!\n");
		return true;
	}

	bool BufferGroup::Shutdown()
	{
		GameLogger::Log(MessageType::Process, "BufferGroup shutdown successful!\n");
		return true;
	}

	bool BufferGroup::AddMesh(Mesh * pMeshToAdd)
	{
		// loop through all buffer infos and check if they have room for this mesh if they do add the mesh to it
		bool foundInfo = false;
		for (unsigned int i = 0; i < m_nextBufferPair; ++i)
		{
			// check if the buffer has room for the mesh
			if (m_bufferPairs[i].HasRoomFor(pMeshToAdd->GetVertexSizeInBytes(), pMeshToAdd->GetIndexSizeInBytes()))
			{
				// if it does, add it, if not keep looking
				if (!m_bufferPairs[i].AddMesh(pMeshToAdd))
				{
					GameLogger::Log(MessageType::cWarning, "Buffer info index [%d] failed to add mesh!\n", i);
					return false;
				}

				foundInfo = true;
				break; // no need to continue processing
			}
		}

		// no buffer info had room for this mesh
		if (!foundInfo)
		{
			if (m_nextBufferPair >= MAX_BUFFER_INFO_PER_GROUP)
			{
				GameLogger::Log(MessageType::cWarning, "BufferGroup failed to add mesh, could not create new buffer info! Limit of [%d] has been reached!\n", MAX_BUFFER_INFO_PER_GROUP);
				return false;
			}

			// create a new one of either the default size or the size the mesh needs, whichever is greater
			GLuint infoSize = m_defaultBuffersize;
			infoSize = ((infoSize < pMeshToAdd->GetVertexSizeInBytes()) ? pMeshToAdd->GetVertexSizeInBytes() : infoSize);
			infoSize = ((infoSize < pMeshToAdd->GetIndexSizeInBytes()) ? pMeshToAdd->GetIndexSizeInBytes() : infoSize);
			m_bufferPairs[m_nextBufferPair].Initialize(infoSize);

			// add the mesh to the new one
			if (!m_bufferPairs[m_nextBufferPair++].AddMesh(pMeshToAdd))
			{
				GameLogger::Log(MessageType::cWarning, "New buffer info index [%d] failed to add mesh!\n", m_nextBufferPair - 1);
				return false;
			}
		}

		return true;
	}

	bool BufferGroup::AddGraphicalObject(GraphicalObject *pGraphicalObjectToAdd)
	{
		// loop through each buffer info and check if the graphical object belongs in that buffer info based on the render info ids
		for (unsigned int i = 0; i < m_nextBufferPair; ++i)
		{
			if (m_bufferPairs[i].BelongsInBuffer(pGraphicalObjectToAdd))
			{
				m_bufferPairs[i].AddGraphicalObject(pGraphicalObjectToAdd);
				return true;
			}
		}

		GameLogger::Log(MessageType::cWarning, "Tried to add graphical object to buffer group but it did not belong in any of the contained buffer infos!\n");
		return false;
	}

	void BufferGroup::RemoveGraphicalObject(GraphicalObject * pGraphicalObjectToRemove)
	{
		// loop through each buffer info and check if the graphical object belongs in that buffer info based on the render info ids
		for (unsigned int i = 0; i < m_nextBufferPair; ++i)
		{
			if (m_bufferPairs[i].BelongsInBuffer(pGraphicalObjectToRemove))
			{
				m_bufferPairs[i].RemoveGraphicalObject(pGraphicalObjectToRemove);
			}
		}
	}

	bool BufferGroup::BelongsInThisGroup(VertexFormat format, GLint shaderProgramID, bool cull)
	{
		// mesh/gob should go in this group if it is of the same vertex format and uses the same shader, and has the same culling state
		return (format == m_vertexFormat) && (m_shaderProgramID == shaderProgramID) && (cull == m_cullForObjectsInBuffer);
	}

	bool BufferGroup::BelongsInThisGroup(Mesh * pMeshToCheck)
	{
		if (!pMeshToCheck) { GameLogger::Log(MessageType::cWarning, "Buffer group could not check if mesh belongs in group. Mesh to be checked was nullptr!\n"); return false; }
		return BelongsInThisGroup(pMeshToCheck->GetVertexFormat(), pMeshToCheck->GetShaderProgramID(), pMeshToCheck->IsCullingEnabledForObject());
	}

	bool BufferGroup::BelongsInThisGroup(GraphicalObject * pGraphicalObjectToCheck)
	{
		if (!pGraphicalObjectToCheck) { GameLogger::Log(MessageType::cWarning, "Buffer group could not check if graphical object belongs in group. GraphicalObject to be checked was nullptr!\n"); return false; }
		return BelongsInThisGroup(pGraphicalObjectToCheck->GetMeshPointer());
	}

	bool BufferGroup::BufferGroupDoesCull()
	{
		return m_cullForObjectsInBuffer;
	}

	GLint BufferGroup::GetNextBufferInfo()
	{
		return m_nextBufferPair;
	}

	BufferInfo * BufferGroup::GetBufferInfos()
	{
		return &m_bufferPairs[0];
	}

	GLint BufferGroup::GetShaderProgramID()
	{
		return m_shaderProgramID;
	}
}

