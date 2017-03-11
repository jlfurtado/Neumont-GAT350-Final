#ifndef VERTEX_H
#define VERTEX_H

#include "GL\glew.h"
#include "Vec3.h"

// Justin Furtado
// 7/31/2016
// Vertex.h
// Struct for colored vertex

namespace Engine
{
	struct Vertex
	{
	public:
		// methods
		static GLuint GetPositionOffset() { return 0; }
		static GLuint GetPositionSize() { return sizeof(m_position); } // Size of position data - only have to change in one place if type changes!
		static GLuint GetStride() { return GetPositionSize(); } // how far in an array of Vertices to go to get to the next vertex

		// data
		Vec3 m_position;
	};
}

#endif // ifndef VERTEX_H