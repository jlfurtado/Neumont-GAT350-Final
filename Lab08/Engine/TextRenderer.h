#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

// Wesley Sheng
// TextRenderer.h
// Renders text.
// Used LearnOpenGL.com for Reference

#include "GL\glew.h"
#include <map>
#include "TextCharacter.h"
#include "Vec3.h"
#include "ShaderProgram.h"

namespace Engine
{
	template class __declspec(dllexport) std::map<GLchar, TextCharacter>;
	class ENGINE_SHARED TextRenderer
	{
	public:
		bool Initialize(int screenWidth, int screenHeight);
		void SetShader(ShaderProgram * shader) { m_shader = shader; }

		ShaderProgram * m_shader;

		bool Load(const char * font, GLuint fontSize);


		void SetScale(GLfloat scale = 1.0f) { m_currentScale = scale; }
		void SetColor(Vec3 color = Vec3{ 1.0f }) { m_currentColor = color; }
		void SetValues(GLfloat scale = 1.0f, Vec3 color = Vec3{ 1.0f }) { m_currentScale = scale; m_currentColor = color; }

		void RenderText(GLfloat x, GLfloat y, const char * text);
		void RenderText(GLfloat x, GLfloat y, GLfloat scale, const char * text);
		void RenderText(GLfloat x, GLfloat y, Vec3 color, const char * text);
		void RenderText(GLfloat x, GLfloat y, GLfloat scale, Vec3 color, const char * text);


		template<typename...Args>
		void RenderText(GLfloat x, GLfloat y, Vec3 color, const char * text, Args... args);

		template<typename...Args>
		void RenderText(GLfloat x, GLfloat y, const char * text, Args... args);
	private:
		std::map<GLchar, TextCharacter> m_characters;

		GLuint m_vao;
		GLuint m_vbo;

		int m_screenWidth;
		int m_screenHeight;

		GLfloat m_currentScale = 1.0f;
		Vec3 m_currentColor = { 1.0f, 1.0f, 1.0f };
	};

	template <typename ... Args>
	void TextRenderer::RenderText(GLfloat x, GLfloat y, Vec3 color, const char* text, Args... args)
	{
		const int bufSize = 400;
		char buffer[bufSize];
		sprintf_s(buffer, bufSize, text, args...);
		RenderText(x, y, color, buffer);
	}

	template<typename ...Args>
	inline void TextRenderer::RenderText(GLfloat x, GLfloat y, const char * text, Args ...args)
	{
		const int bufSize = 400;
		char buffer[bufSize];
		sprintf_s(buffer, bufSize, text, args...);
		RenderText(x, y, buffer);
	}

}
#endif TEXTRENDERER_H_

