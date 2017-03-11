#ifndef BUFFERINFO_H
#define BUFFERINFO_H

// Justin Furtado
// 7/31/2016
// BufferInfo.h
// A vertex and index buffer

#include "Mesh.h"
#include "VertexFormat.h"

namespace Engine
{
	class GraphicalObject;
	class BufferInfo
	{
	public:
		BufferInfo();
		~BufferInfo();

		bool Initialize(GLuint bufferSize);
		bool Shutdown();

		bool AddMesh(Mesh *pMeshToAdd);
		bool AddGraphicalObject(GraphicalObject *pGraphicalObjectToAdd);
		void RemoveGraphicalObject(GraphicalObject *pGraphicalObjectToRemove);
		bool HasRoomFor(GLuint vertexSizeBytes, GLuint indexSizeBytes);
		bool BelongsInBuffer(GLuint vertexBufferID, GLuint indexBufferID);
		bool BelongsInBuffer(Mesh *pMeshToCheck);
		bool BelongsInBuffer(GraphicalObject *pGraphicalObjectToCheck);
		GraphicalObject *GetHeadNode();
		GLuint GetVertexBufferID();
		GLuint GetIndexBufferID();

	private:
		// default buffer size for all buffer info from config file here...
		GraphicalObject *m_pHeadNode;
		GLuint m_vertexBufferID;
		GLuint m_indexBufferID;
		GLuint m_vertexBufferOffsetBytes;
		GLuint m_indexBufferOffsetBytes;
		GLuint m_bufferSize;
		GLuint m_vertexCount;
		GLuint m_indexCount;
		GLuint m_meshCount;
		GLuint m_graphicalObjectCount;
	};
}

#endif // ifndef BUFFERINFO_H