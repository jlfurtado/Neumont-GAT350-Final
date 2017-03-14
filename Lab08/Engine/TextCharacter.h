#ifndef TEXT_CHARACTER_H_
#define TEXT_CHARACTER_H_

// Wesley Sheng
// TextCharacter.h
// Holds Character information for text rendering

#include "GL\glew.h"
#include "Vec2.h"

namespace Engine
{
	struct ENGINE_SHARED TextCharacter {
		GLuint     TextureID; // ID handle of glyph texture
		Vec2	   Size;	  // Size of glyph
		Vec2	   Bearing;	  // Offset from baseline to left/top of glyph
		GLuint     Advance;	  // Offset to advance to next glyph
	};
}

#endif // ndef TEXT_CHARACTER_H_