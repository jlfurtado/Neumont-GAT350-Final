#ifndef RENDERINFO_H
#define RENDERINFO_H

// Justin Furtado
// 7/14/2016
// RenderInfo.h
// Represents information needed by meshes to be rendered by RenderEngine

#include "ExportHeader.h"

namespace Engine
{
	struct ENGINE_SHARED RenderInfo
	{
	public:
		GLuint vertexBufferID;
		GLuint indexBufferID;
		GLuint vertexBufferOffset;
		GLuint indexBufferOffset;

	};
}

#endif // ifndef RENDERINFO_H