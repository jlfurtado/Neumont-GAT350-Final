#ifndef BUFFERGROUP_H
#define BUFFERGROUP_H

// Justin Furtado
// 7/31/2016
// BufferGroup.h
// Holds BufferInfo of similar state in a group

#include "BufferInfo.h"
#include "VertexFormat.h"

namespace Engine
{
	class GraphicalObject;
	class BufferGroup
	{
	public:
		BufferGroup();
		~BufferGroup();

		bool Initialize(GLuint defaultBufferSize, VertexFormat bufferGroupVertexFormat, GLint shaderProgramID, bool cullForObjectsInBuffer);
		bool Shutdown();

		bool AddMesh(Mesh *pMeshToAdd);
		bool AddGraphicalObject(GraphicalObject *pGraphicalObjectToAdd);
		void RemoveGraphicalObject(GraphicalObject *pGraphicalObjectToRemove);

		bool BelongsInThisGroup(VertexFormat format, GLint shaderProgramID, bool cull);
		bool BelongsInThisGroup(Mesh *pMeshToCheck);
		bool BelongsInThisGroup(GraphicalObject *pGraphicalObjectToCheck);

		bool BufferGroupDoesCull();
		GLint GetNextBufferInfo();
		BufferInfo *GetBufferInfos();
		GLint GetShaderProgramID();

	private:
		static const int MAX_BUFFER_INFO_PER_GROUP = 20;
		bool m_cullForObjectsInBuffer;
		GLuint m_defaultBuffersize;
		GLuint m_nextBufferPair;
		GLint m_shaderProgramID;
		VertexFormat m_vertexFormat;
		BufferInfo m_bufferPairs[MAX_BUFFER_INFO_PER_GROUP];
	};
}

#endif // ifndef BUFFERGROUP_H