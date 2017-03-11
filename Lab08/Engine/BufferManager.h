#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "GL\glew.h"

#include "Mesh.h"
#include "BufferGroup.h"

// Justin Furtado
// 7/14/2016
// BufferManager.h
// Manages OpenGL buffers

namespace Engine
{
	class GraphicalObject;
	class ShaderProgram;
	class BufferManager
	{
	public:
		// methods
		static bool Initialize();
		static bool Shutdown();
		static bool AddMesh(Mesh * pMeshToAdd);
		static bool AddGraphicalObject(GraphicalObject * pGraphicalObjectToAdd);
		static void RemoveGraphicalObject(GraphicalObject *pGraphicalObjectToRemove);
		static void ConsoleLogStats();
		static GLint GetNextBufferGroup();
		static BufferGroup *GetBufferGroups();
	private:
		// methods
		static bool ReadConfigValues();

		// data
		static const GLint MIN_BUFFER_SIZE = 4086; // 4 kilos
		static const GLint DEFAULT_BUFFER_SIZE = 1024 * 1024; // 1 meg
		static const GLint MAX_BUFFER_SIZE = 1024 * 1024 * 16; // 16 meg

		static const GLint MAX_BUFFER_GROUPS = 20;
		static BufferGroup s_bufferGroups[MAX_BUFFER_GROUPS];
		static GLint s_nextBufferGroup;

		static GLuint s_vertexCount;
		static GLuint s_indexCount;
		static GLuint s_meshCount;
		static GLuint s_graphicalObjectCount;
		static GLint s_bufferSize;
	};
}

#endif // ifndef BUFFERMANAGER_H