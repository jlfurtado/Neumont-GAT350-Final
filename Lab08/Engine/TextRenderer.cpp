#include "TextRenderer.h"

#include "GameLogger.h"
#include <ft2build.h>
#include FT_FREETYPE_H


#include "MyGL.h"
#include <string>

#pragma warning(push)
#pragma warning(disable : 4201)
#include "Mat4.h"
#pragma warning(pop)

namespace Engine
{
	bool TextRenderer::Initialize(int width, int height)
	{
		// Shader should be good!
		glUseProgram(m_shader->GetProgramId());
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glUseProgram() failed.")) return false;
		glGenVertexArrays(1, &m_vao);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glGenVertexArrays() failed.")) return false;
		glGenBuffers(1, &m_vbo);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glGenBuffers() failed.")) return false;
		glBindVertexArray(m_vao);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glBindVertexArray() failed.")) return false;
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glBindBuffer() failed.")) return false;
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glBufferData() failed.")) return false;
		glEnableVertexAttribArray(0);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glEnableVertexAttribArray() failed.")) return false;
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glVertexAttribPointer() failed.")) return false;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glBindBuffer(%d) failed."), 0) return false;
		glBindVertexArray(0);
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glBindVertexArray(%d) failed."), 0) return false;

		m_screenWidth = width;
		m_screenHeight = height;

		return true;
	}

	bool TextRenderer::Load(const char * font, GLuint fontSize)
	{
		glUseProgram(m_shader->GetProgramId());
		m_characters.clear();

		GLint uniformProjection = m_shader->GetUniformLocation("projection");

		int width = m_screenWidth;
		int height = m_screenHeight;
		Mat4 matProjection = Mat4::Orthographic(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, matProjection.GetAddress());
		if (MyGL::TestForError(MessageType::cFatal_Error, "TextRenderer::Initialize() glUniformMatrix4fv() failed.")) return false;

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			GameLogger::Log(MessageType::cError, "TextRenderer::RenderText() could not initialize FreeType Library.");
			return false;
		}
		FT_Face face;
		if (FT_New_Face(ft, font, 0, &face))
		{
			GameLogger::Log(MessageType::cError, "TextRenderer::RenderText() could not load font.");
			return false;
		}
		FT_Set_Pixel_Sizes(face, 0, fontSize);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (GLubyte c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, (char)c, FT_LOAD_RENDER))
			{
				GameLogger::Log(MessageType::Error, "TextRenderer::RenderText() failed to load Glyph %d", c);
				continue;
			}
			FT_GlyphSlot glyph = face->glyph;

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				glyph->bitmap.width,
				glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			TextCharacter character = {
				texture,
				Vec2(static_cast<float>(glyph->bitmap.width), static_cast<float>(glyph->bitmap.rows)),
				Vec2(static_cast<float>(glyph->bitmap_left),  static_cast<float>(glyph->bitmap_top)),
				static_cast<GLuint>(glyph->advance.x)
			};
			m_characters.insert(std::pair<GLchar, TextCharacter>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		return true;
	}

	void TextRenderer::RenderText(GLfloat x, GLfloat y, const char * text)
	{
		const char *p;

		// m_shader->LinkAndUseProgram();
		glUseProgram(m_shader->GetProgramId());

		GLint textColor = m_shader->GetUniformLocation("textColor");

		glUniform3f(textColor, m_currentColor.GetX(), m_currentColor.GetY(), m_currentColor.GetZ());;

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(m_vao);

		// Iterate through all characters
		for (p = text; *p; p++)
		{
			TextCharacter ch = m_characters[*p];

			GLfloat xpos = x + ch.Bearing.GetX() * m_currentScale;
			GLfloat ypos = y + (this->m_characters['H'].Bearing.GetY() - ch.Bearing.GetY()) * m_currentScale;

			GLfloat w = ch.Size.GetX() * m_currentScale;
			GLfloat h = ch.Size.GetY() * m_currentScale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos    , ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos    ,   1.0, 0.0 },
				{ xpos    , ypos    ,   0.0, 0.0 },

				{ xpos,     ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 1.0 },
				{ xpos + w, ypos    ,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			GLint uniformText = m_shader->GetUniformLocation("text");
			glUniform1i(uniformText, 0);

			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph
			x += (ch.Advance >> 6) * m_currentScale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextRenderer::RenderText(GLfloat x, GLfloat y, GLfloat scale, const char * text)
	{
		m_currentScale = scale;
		RenderText(x, y, text);
	}

	void TextRenderer::RenderText(GLfloat x, GLfloat y, Vec3 color, const char * text)
	{
		m_currentColor = color;
		RenderText(x, y, text);
	}

	void TextRenderer::RenderText(GLfloat x, GLfloat y, GLfloat scale, Vec3 color, const char * text)
	{
		m_currentScale = scale;
		m_currentColor = color;
		RenderText(x, y, text);
	}
}
