#include "BufferInfo.h"
#include "BufferManager.h"
#include "MyGL.h"
#include "GraphicalObject.h"
#include "ColorVertex.h" // todo remove if possible

// Justin Furtado
// 7/31/2016
// BufferInfo.cpp 
// A vertex and index buffer

namespace Engine
{
	BufferInfo::BufferInfo()
		: m_pHeadNode(nullptr), m_vertexBufferID(0), m_indexBufferID(0), m_vertexBufferOffsetBytes(0), m_indexBufferOffsetBytes(0),
		m_bufferSize(0), m_vertexCount(0), m_indexCount(0), m_graphicalObjectCount(0), m_meshCount(0)
	{
	}

	BufferInfo::~BufferInfo()
	{
	}

	bool BufferInfo::Initialize(GLuint bufferSize)
	{
		m_bufferSize = bufferSize;

		// Generate the vertex buffer and check for errors
		glGenBuffers(1, &m_vertexBufferID);
		if (MyGL::TestForError(MessageType::cFatal_Error, "Buffer Info Initialize Gen Vertex Buffer Errors") || !m_vertexBufferID)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Buffer Info was unable to generate a vertex buffer!\n");
			return false;
		}

		// bind the vertex buffer and check for errors
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
		if (MyGL::TestForError(MessageType::cFatal_Error, "Buffer Info Initialize Bind Vertex Buffer Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Buffer Info was unable to bind the vertex buffer with id [%d]!\n", m_vertexBufferID);
			return false;
		}

		// initialize the vertex buffer data and check for errors
		glBufferData(GL_ARRAY_BUFFER, m_bufferSize, nullptr, GL_STATIC_DRAW);
		if (MyGL::TestForError(MessageType::cFatal_Error, "Buffer Info Initialize Vertex Buffer Data Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Buffer Info was unable to create an empty vertex buffer of size [%d] bytes!\n", m_bufferSize);
			return false;
		}

		// Generate the index buffer and check for errors
		glGenBuffers(1, &m_indexBufferID);
		if (MyGL::TestForError(MessageType::cFatal_Error, "Buffer Info Initialize Gen Index Buffer Errors") || !m_indexBufferID)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Buffer Info was unable to generate an index buffer!\n");
			return false;
		}

		// Bind the index buffer and check for errors
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
		if (MyGL::TestForError(MessageType::cFatal_Error, "Buffer Info Bind Index Buffer Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Buffer Info was unable to bind the index buffer with id [%d]!\n", m_indexBufferID);
			return false;
		}

		// initialize the index buffer data and check for errors
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_bufferSize, nullptr, GL_STATIC_DRAW);
		if (MyGL::TestForError(MessageType::cFatal_Error, "Buffer Info Initialize Index Buffer Data Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Buffer Info was unable to create an empty index buffer of size [%d] bytes!\n", m_bufferSize);
			return false;
		}

		// indicate success
		GameLogger::Log(MessageType::Process, "Buffer Info initialized successfully!\n");
		return true;
	}

	bool BufferInfo::Shutdown()
	{
		// indicate success
		GameLogger::Log(MessageType::Process, "Buffer Info shutdown successfully!\n");
		return true;
	}

	bool BufferInfo::HasRoomFor(GLuint vertexSizeBytes, GLuint indexSizeBytes)
	{
		// size - used  = amount of space left, check if there is enough space in both buffers
		return ((m_bufferSize - m_vertexBufferOffsetBytes) >= vertexSizeBytes) && ((m_bufferSize - m_indexBufferOffsetBytes) >= indexSizeBytes);
	}

	bool BufferInfo::BelongsInBuffer(GLuint vertexBufferID, GLuint indexBufferID)
	{
		// belongs in the buffer if ids match
		return (m_vertexBufferID == vertexBufferID) && (m_indexBufferID == indexBufferID);
	}

	bool BufferInfo::BelongsInBuffer(Mesh * pMeshToCheck)
	{
		if (!pMeshToCheck) { GameLogger::Log(MessageType::cWarning, "Buffer Info could not check if mesh belongs in buffer! Mesh to check was nullptr!\n"); return false; }
		return BelongsInBuffer(pMeshToCheck->GetRenderInfoPtr()->vertexBufferID, pMeshToCheck->GetRenderInfoPtr()->indexBufferID);
	}

	bool BufferInfo::BelongsInBuffer(GraphicalObject * pGraphicalObjectToCheck)
	{
		if (!pGraphicalObjectToCheck) { GameLogger::Log(MessageType::cWarning, "Buffer Info could not check if graphical object belongs in buffer! Graphical object to check was nullptr!\n"); return false; }
		return BelongsInBuffer(pGraphicalObjectToCheck->GetMeshPointer());
	}

	GraphicalObject * BufferInfo::GetHeadNode()
	{
		return m_pHeadNode;
	}

	GLuint BufferInfo::GetVertexBufferID()
	{
		return m_vertexBufferID;
	}

	GLuint BufferInfo::GetIndexBufferID()
	{
		return m_indexBufferID;
	}

	bool BufferInfo::AddMesh(Mesh *pMeshToAdd)
	{
		// Make sure we're using the right buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);

		// All the error checking
		if (MyGL::TestForError(MessageType::cError, "Buffer Info AddMesh Vertex Buffer Bind Errors"))
		{
			GameLogger::Log(MessageType::cError, "Buffer Info failed to AddMesh. Could not bind vertex buffer with ID [%d]!\n", m_vertexBufferID);
			return false;
		}

		// Copy data from mesh into vertex buffer
		glBufferSubData(GL_ARRAY_BUFFER, m_vertexBufferOffsetBytes, pMeshToAdd->GetVertexSizeInBytes(), pMeshToAdd->GetVertexPointer());

		// Even more error checking
		if (MyGL::TestForError(MessageType::cError, "Buffer Info AddMesh Vertex Buffer SubData Errors"))
		{
			GameLogger::Log(MessageType::cError, "Buffer Info failed to AddMesh. Could not copy [%d] bytes of vertex data into buffer location [%d] from location [%p]!\n", pMeshToAdd->GetVertexSizeInBytes(), m_vertexBufferOffsetBytes, pMeshToAdd->GetVertexPointer());
			return false;
		}

		if (pMeshToAdd->IsIndexed())
		{
			// Make sure we're using the right buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);

			// Yet another error check
			if (MyGL::TestForError(MessageType::cError, "Buffer Info AddMesh Index Buffer Bind Errors"))
			{
				GameLogger::Log(MessageType::cError, "Buffer Info failed to AddMesh. Could not bind index buffer with ID [%d]!\n", m_indexBufferID);
				return false;
			}

			// Copy data into index buffer
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferOffsetBytes, pMeshToAdd->GetIndexSizeInBytes(), pMeshToAdd->GetIndexPointer());

			// One last error check
			if (MyGL::TestForError(MessageType::cError, "Buffer Info AddMesh Index Buffer SubData Errors"))
			{
				GameLogger::Log(MessageType::cError, "Buffer Info failed to AddMesh. Could not copy [%d] bytes of index data into buffer location [%d] from location [%p]", pMeshToAdd->GetIndexSizeInBytes(), m_indexBufferOffsetBytes, pMeshToAdd->GetIndexPointer());
				return false;
			}
		}

		// Set render info
		pMeshToAdd->GetRenderInfoPtr()->vertexBufferID = m_vertexBufferID;
		pMeshToAdd->GetRenderInfoPtr()->vertexBufferOffset = m_vertexBufferOffsetBytes;
		pMeshToAdd->GetRenderInfoPtr()->indexBufferID = m_indexBufferID;
		pMeshToAdd->GetRenderInfoPtr()->indexBufferOffset = m_indexBufferOffsetBytes;

		// shift buffer offsets over so the next thing is added in the right spot
		m_vertexBufferOffsetBytes += pMeshToAdd->GetVertexSizeInBytes();
		m_indexBufferOffsetBytes += pMeshToAdd->GetIndexSizeInBytes();

		// update statistics to be logged
		m_indexCount += pMeshToAdd->GetIndexCount();
		m_vertexCount += pMeshToAdd->GetVertexCount();
		m_meshCount++;

		GameLogger::Log(MessageType::cDebug, "[%d] of [%d] bytes used in vertex buffer [%d]! [%d] bytes remain!\n", m_vertexBufferOffsetBytes, m_bufferSize, m_vertexBufferID, m_bufferSize - m_vertexBufferOffsetBytes);
		if (pMeshToAdd->IsIndexed()) { GameLogger::Log(MessageType::cDebug, "[%d] of [%d] bytes used in index buffer [%d]! [%d] bytes remain!\n", m_indexBufferOffsetBytes, m_bufferSize, m_indexBufferID, m_bufferSize - m_indexBufferOffsetBytes); }

		GameLogger::Log(MessageType::Process, "BufferInfo successfully added mesh!\n");
		return true;
	}

	bool BufferInfo::AddGraphicalObject(GraphicalObject *pGraphicalObjectToAdd)
	{
		// Add graphical object to front of linked list
		pGraphicalObjectToAdd->PointToDrawingNode(m_pHeadNode);
		m_pHeadNode = pGraphicalObjectToAdd;

		// Update statistics
		m_graphicalObjectCount++;

		GameLogger::Log(MessageType::Process, "BufferInfo successfully added GraphicalObject located at [%p]!\n", pGraphicalObjectToAdd);
		return true;
	}

	void BufferInfo::RemoveGraphicalObject(GraphicalObject * pGraphicalObjectToRemove)
	{
		GraphicalObject *pCurrentNode = m_pHeadNode;
		if (m_pHeadNode == pGraphicalObjectToRemove) { m_pHeadNode = m_pHeadNode->GetNextDrawingNode(); return; }

		while (pCurrentNode)
		{
			if (pCurrentNode->GetNextDrawingNode() == pGraphicalObjectToRemove) { pCurrentNode->PointToDrawingNode(pGraphicalObjectToRemove->GetNextDrawingNode()); return; }
			pCurrentNode = pCurrentNode->GetNextDrawingNode();
		}
	}
}

