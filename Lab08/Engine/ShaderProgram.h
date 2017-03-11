#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

// Justin Furtado
// 6/26/2016
// ShaderProgram.h
// Wrapper class for an OpenGL Shader Program

#include <GL\glew.h>
#include "ExportHeader.h"

namespace Engine
{
	const int maxShaderNameLen = 256;  // relative path to shader
	const int maxShaders = 3;    // only need vertex, geometry and frag shaders for now

	struct ENGINE_SHARED ShaderInfo
	{
		char   name[maxShaderNameLen]{ 0 }; // name[0] == '\0' if this shader info is empty
		GLuint id{ 0 };
		GLenum type{ 0 };      // shader type, eg, GL_VERTEX_SHADER
	};

	class ENGINE_SHARED ShaderProgram
	{
	public:
		bool Initialize();
		bool Shutdown();

		// Shader methods
		bool AddVertexShader(const char* const filename);
		bool AddGeometryShader(const char *const fileName);
		bool AddFragmentShader(const char* const filename);
		bool AddShader(const char* const filename, GLenum shaderType); // prefer adding specific shaders
		bool CheckShaderCompileStatus(GLuint shaderID);
		bool IsLinked() const;

		// Program methods
		bool CheckProgramStatus(GLenum pname);
		bool CheckProgramStatus();
		bool LinkProgram();
		bool UseProgram();
		bool LinkAndUseProgram();
		GLint GetUniformLocation(const GLchar* name);
		GLint GetAttribLocation(const GLchar* name);
		GLint GetSubroutineIndex(GLenum shaderType, const GLchar* name);
		GLint GetProgramId() const { return m_id; }

		// shader info methods
		bool ParseShader();
		bool ShowActiveAttributes();
		bool ShowActiveUniforms(bool showUniformBlocks);

	private:
		void ShowShaderLogInfo(GLuint shaderID, char* infoBuffer, GLint bufferLen);
		void ShowProgramLogInfo(char* infoBuffer, GLint bufferLen, GLenum pname);
		bool SaveShaderInfo(const char* const filename, GLenum shaderType, GLuint shaderId);
		bool DeleteShaders();
		bool DeleteProgram();

		int GetNumAttributes();
		int GetNumUniforms();
		bool DisplaySingleAttribute(int attribNum);
		bool DisplaySingleUniform(int uniformNum, bool displayUniformBlock);
		bool DisplayShaderInfoHeader();
		const char *GetTypeString(GLenum type);

	private:
		GLuint m_id;
		GLboolean m_deleted;
		bool m_isLinked{ false };
		GLboolean m_shaderDeleted[maxShaders];
		ShaderInfo m_shaders[maxShaders];
	};
}

#endif // ifndef SHADERPROGRAM_H
