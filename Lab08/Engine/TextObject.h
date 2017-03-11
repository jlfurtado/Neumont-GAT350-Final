#ifndef TextObject_H
#define TextObject_H

// Justin Furtado
// 7/6/2016
// TextObject.h
// Draws text to the screen!!!

#include "GL\glew.h"
#include "Vertex.h"
#include "ExportHeader.h"

namespace Engine
{
	// Struct shows how interleaved data from stb is stored
	struct ENGINE_SHARED STBDatum
	{
		GLfloat x, y, z;
		GLubyte r, g, b, a;
	};

	class ShaderProgram;
	class ENGINE_SHARED TextObject
	{
	public:
		static bool Initialize(GLint matLoc, GLint tintLoc);
		void SetupText(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat rValue, GLfloat gValue, GLfloat bValue, GLfloat fontScaleX, GLfloat fontScaleY, GLchar *text);
		void RenderText(ShaderProgram *pShaderProgram, GLint debugColorLoc);
		static bool Shutdown();

	private:
		// methods
		void ConvertData(GLfloat xPos, GLfloat yPos, GLfloat zPos, STBDatum *pSTBData, Vertex *pGLData, GLuint *pGLIndices, GLuint quadCount);

		// data
		static const int MAX_STRING_SIZE = 250; // Largest number of characters expected to be drawn!
		static const int STB_BUFFER_SIZE_BYTES = 500 * MAX_STRING_SIZE; // Documentation says expect 270 bytes per char to be drawn, so I rounded up to be safe TODO: investigate possible errors here, had to increase from 300 to 500 to get messages to display properly
		static const int STB_BUFFER_SIZE_COUNT = STB_BUFFER_SIZE_BYTES / (sizeof(STBDatum));
		static const int VERTEX_BUFFER_SIZE_BYTES = 6 * sizeof(GLfloat)*STB_BUFFER_SIZE_COUNT; // (6 floats in ColorVertex)/sizeof(STB) gives proportion, make max proportionate to max stb
		static const int VERTEX_BUFFER_SIZE_COUNT = VERTEX_BUFFER_SIZE_BYTES / sizeof(Vertex);
		static const int INDEX_BUFFER_SIZE_BYTES = (6 * sizeof(GLuint)*STB_BUFFER_SIZE_COUNT) / 4; // account for float-to-ushort size difference and for the fact that there are 6 indices per 4 vertices
		static const int INDEX_BUFFER_SIZE_COUNT = INDEX_BUFFER_SIZE_BYTES / sizeof(GLuint);
		static const int FLOATS_PER_VERTEX = 6;
		static const float BASE_FONT_SCALE;
		static GLuint s_textBufferID;
		static GLuint s_textIndexBufferID;
		static GLint s_matLoc;
		static GLint s_tintLoc;
		Vertex m_vertices[VERTEX_BUFFER_SIZE_COUNT];
		GLuint m_indices[INDEX_BUFFER_SIZE_COUNT];
		GLint m_numQuads;
		GLfloat m_fontScaleX;
		GLfloat m_fontScaleY;
		Vec3 m_textColor;
	};
}

#endif // ifndef TextObject_H