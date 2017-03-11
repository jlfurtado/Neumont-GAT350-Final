#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include "ExportHeader.h"
#include "GL\glew.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "BufferGroup.h"
#include "BufferInfo.h"
#include "Mat4.h"
#include "InstanceBuffer.h"

// Justin Furtado
// 7/14/2016
// RenderEngine.h
// Manages the rendering of graphical objects 

namespace Engine
{
	class ENGINE_SHARED RenderEngine
	{
	public:
		static bool Initialize(ShaderProgram *pShaderPrograms, GLint programCount);
		static bool Shutdown();
		static bool AddMesh(Mesh *pMeshToAdd);
		static bool AddGraphicalObject(GraphicalObject *pGraphicalObjectToAdd);
		static void RemoveGraphicalObject(GraphicalObject *pGraphicalObjectToRemove);
		static bool Draw();
		static bool DrawSingleObjectDifferently(GraphicalObject *pGob, void *pPersp, void *pLook, void *pTexId, int lookLoc, int perspLoc, int texLoc);
		static bool DrawSingleObjectRegularly(GraphicalObject * pGob);
		static bool DrawSingleObjectWithDifferentMeshMode(GraphicalObject * pGob, GLenum meshMode);
		static bool DrawInstanced(GraphicalObject *pGob, InstanceBuffer *pInstanceBuffer);
		static bool DrawInstanced(GraphicalObject *pGob, int count);

		static void LogStats();

	private:
		static const int MAX_SHADER_PROGRAMS = 20;
		static GLenum GetIndexType(IndexSizeInBytes indexSize);
		static bool SetupDrawingEnvironment(BufferInfo *pBufferInfo, int *outIndex = nullptr);
		static bool SetupAttribs(BufferInfo *pBufferInfo, int *outIndex = nullptr);
		static bool CopyShaderPrograms(ShaderProgram *pShaderPrograms, GLint shaderProgramCount);
		static ShaderProgram *GetShaderProgramByID(GLint shaderProgramID);
		static ShaderProgram s_shaderPrograms[MAX_SHADER_PROGRAMS];
		static GLuint s_nextShaderProgram;
	};
}

#endif // ifndef RENDERENGINE_H