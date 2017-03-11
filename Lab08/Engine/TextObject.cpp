#include "TextObject.h"
#include "Mat4.h"

// Justin Furtado
// 7/6/2016
// TextObject.cpp
// Draws text to the screen!!!

#pragma warning(push)
#pragma warning(disable : 4505)
#include "stb_easy_font.h"
#pragma warning(pop)

#include "ShaderProgram.h"
#include "MyGL.h"
#include "StringFuncs.h"

namespace Engine
{
	const int INDICES_PER_QUAD = 6;
	const int VERTICES_PER_QUAD = 4;

	GLuint TextObject::s_textBufferID = 0;
	GLuint TextObject::s_textIndexBufferID = 0;
	GLint TextObject::s_matLoc = -1;
	GLint TextObject::s_tintLoc = -1;
	const float TextObject::BASE_FONT_SCALE = 1.0f / 100.0f;

	bool TextObject::Initialize(GLint matLoc, GLint tintLoc)
	{
		// setup buffers for data
		glGenBuffers(1, &s_textBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, s_textBufferID);
		glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE_BYTES, nullptr, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);

		// setup indices for buffers (file stores in quads according to doc, must use indices to convert to tris!)
		glGenBuffers(1, &s_textIndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_textIndexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFFER_SIZE_BYTES, nullptr, GL_STATIC_DRAW);

		// check for gl errors
		if (MyGL::TestForError(MessageType::cFatal_Error, "Failed to setup buffers for text renderer!\n"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to initialize TextObject - MyGL found errors!\n");
			return false;
		}

		s_matLoc = matLoc;
		s_tintLoc = tintLoc;

		// hooray all is good
		GameLogger::Log(MessageType::Process, "TextObject initialized successfully!\n");
		return true;
	}

	void TextObject::SetupText(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat rValue, GLfloat gValue, GLfloat bValue, GLfloat fontScaleX, GLfloat fontScaleY, GLchar * text)
	{
		// Defensive coding
		if (StringFuncs::StringLen(text) > MAX_STRING_SIZE)
		{
			GameLogger::Log(MessageType::Warning, "String renderer will truncate message [%s] because it is more than [%d] characters!\n", text, MAX_STRING_SIZE);
		}

		// set data
		m_fontScaleX = fontScaleX;
		m_fontScaleY = fontScaleY;

		// ==========================
		// Get data from stb
		// ==========================

		// make buffer to copy the quad verts from stb into
		STBDatum quadData[STB_BUFFER_SIZE_COUNT];

		m_numQuads = stb_easy_font_print(xPos, yPos, text, nullptr, &quadData[0], STB_BUFFER_SIZE_COUNT); // stb_print returns the number of quads used for the text

		m_textColor = Vec3(rValue, gValue, bValue);

		// ================================================
		// Convert the data to a more float-y format,
		// fill in zPos space and fix y-axis
		// ================================================
		ConvertData(xPos, yPos, zPos, &quadData[0], &m_vertices[0], &m_indices[0], m_numQuads);
	}

	void TextObject::RenderText(ShaderProgram *pShaderProgram, GLint debugColorLoc)
	{
		GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE); // check if culling is enabled

		// don't cull text
		glDisable(GL_CULL_FACE);

		pShaderProgram->UseProgram();

		// Buffers already created, but they need to be sub-data-ed and drawn
		// use current buffers incase outside binds different buffers
		glBindBuffer(GL_ARRAY_BUFFER, s_textBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_vertices[0].GetStride(), (void *)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_textIndexBufferID);

		// Update them with the data for this text
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_numQuads*VERTICES_PER_QUAD*FLOATS_PER_VERTEX*sizeof(GLfloat), &m_vertices[0]);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (m_numQuads*INDICES_PER_QUAD*sizeof(GLuint)), &m_indices[0]);

		// Draw the data for this text
		Mat4 temp = Mat4();
		glUniformMatrix4fv(13, 1, GL_FALSE, temp.GetAddress());
		glUniformMatrix4fv(16, 1, GL_FALSE, temp.GetAddress());
		glUniformMatrix4fv(17, 1, GL_FALSE, temp.GetAddress());
		glUniform3f(s_tintLoc, m_textColor.GetR(), m_textColor.GetG(), m_textColor.GetB());
		glUniform3f(debugColorLoc, m_textColor.GetR(), m_textColor.GetG(), m_textColor.GetB());
		glDrawElements(GL_TRIANGLES, m_numQuads*INDICES_PER_QUAD, GL_UNSIGNED_INT, 0); // no offset

		// restore culling to previous state
		if (cullEnabled) { glEnable(GL_CULL_FACE); }
	}

	bool TextObject::Shutdown()
	{
		// TODO: delete buffers or whatever you're supposed to do here
		GameLogger::Log(MessageType::Process, "TextObject Shutdown Successfully!!!\n");
		return true;
	}

	void TextObject::ConvertData(GLfloat xPos, GLfloat yPos, GLfloat zPos, STBDatum * pSTBData, Vertex * pGLData, GLuint * pGLIndices, GLuint quadCount)
	{
		// for each vertex
		for (GLuint i = 0; i < quadCount*VERTICES_PER_QUAD; ++i)
		{
			// copy the position values
			(pGLData + i)->m_position.GetAddress()[0] = xPos + ((pSTBData + i)->x) * BASE_FONT_SCALE * m_fontScaleX; // maintain the x position of the data from STB
			(pGLData + i)->m_position.GetAddress()[1] = yPos - ((pSTBData + i)->y * BASE_FONT_SCALE * m_fontScaleY); //(yPos - (pSTBData + i)->y); // STB thinks of Y+ as down, so this is adjusted for here
			(pGLData + i)->m_position.GetAddress()[2] = zPos; // keep our Z because STB doesn't seem to do anything but reserve space for it
		}

		// setup indices
		for (GLuint i = 0; i < quadCount; ++i)
		{
			*(pGLIndices + i*INDICES_PER_QUAD + 0) = i*VERTICES_PER_QUAD + 0;
			*(pGLIndices + i*INDICES_PER_QUAD + 1) = i*VERTICES_PER_QUAD + 1;
			*(pGLIndices + i*INDICES_PER_QUAD + 2) = i*VERTICES_PER_QUAD + 2;
			*(pGLIndices + i*INDICES_PER_QUAD + 3) = i*VERTICES_PER_QUAD + 0;
			*(pGLIndices + i*INDICES_PER_QUAD + 4) = i*VERTICES_PER_QUAD + 2;
			*(pGLIndices + i*INDICES_PER_QUAD + 5) = i*VERTICES_PER_QUAD + 3;
		}
	}
}
