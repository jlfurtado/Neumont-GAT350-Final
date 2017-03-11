#ifndef COLORVERTEX_H
#define COLORVERTEX_H

// Justin Furtado
// 7/6/2016
// ColorVertex.h
// Struct for colored vertex

#include "Vec3.h"

namespace Engine
{
	struct ColorVertex
	{
	public:
		// methods
		static GLuint GetColorOffset() { return GetPositionSize(); } // how many bytes from beginning to get to color data (users of struct need not care of order of data within struct changes)
		static GLuint GetPositionOffset() { return 0; } // For symmetry/readability
		static GLuint GetColorSize() { return sizeof(m_color); } // Size of color data - only have to change in one place if type changes!
		static GLuint GetPositionSize() { return sizeof(m_position); } // Size of position data - only have to change in one place if type changes!
		static GLuint GetStride() { return GetColorSize() + GetPositionSize(); } // how far in an array of ColorVertex to go to get to next ColorVertex

		// data
		Vec3 m_position;
		Vec3 m_color;
	};
}


#endif //ifndef COLORVERTEX_H