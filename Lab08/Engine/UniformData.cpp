#include "UniformData.h"
#include "GL\glew.h"
#include "GameLogger.h"
#include <assert.h>
#include "MyGL.h"

namespace Engine
{
	UniformData::UniformData()
		: m_uniformType(GL_FLOAT_VEC3), m_pUniformData(nullptr), m_uniformDataLoc(0)
	{
	}

	UniformData::UniformData(GLenum type, void * dataAddress, int dataLoc, bool log)
		: m_uniformType(type), m_pUniformData(dataAddress), m_uniformDataLoc(dataLoc), m_logForThis(log)
	{
	}

	bool UniformData::PassUniform()
	{
		if (!m_pUniformData) { GameLogger::Log(MessageType::cError, "PassUniform called but data address is nullptr!\n"); return false; }
		if (m_uniformDataLoc <= 0) { GameLogger::Log(MessageType::cError, "PassUniform called but data location was less than or equal to zero!\n"); return false; 	}

		switch (m_uniformType)
		{
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(m_uniformDataLoc, 1, GL_FALSE, reinterpret_cast<float*>(m_pUniformData));
			/*TODO CHECK THIS
			if (m_logForThis) { GameLogger::Log(MessageType::ConsoleOnly, "Passed (%.3f, %.3f, %.3f, %.3f)\n\t\t\t\t(%.3f, %.3f, %.3f, %.3f)\n\t\t\t\t(%.3f, %.3f, %.3f, %.3f)\n\t\t\t\t(%.3f, %.3f, %.3f, %.3f) to uniform [%d]\n", *(reinterpret_cast<float*>(m_pUniformData) + 0), *(reinterpret_cast<float*>(m_pUniformData) + 4), *(reinterpret_cast<float*>(m_pUniformData) + 8), *(reinterpret_cast<float*>(m_pUniformData) + 12), 				 																																											 *(reinterpret_cast<float*>(m_pUniformData) + 3), *(reinterpret_cast<float*>(m_pUniformData) + 7), *(reinterpret_cast<float*>(m_pUniformData) + 11), *(reinterpret_cast<float*>(m_pUniformData) + 15), m_uniformDataLoc); }

			
			*/
			break;

		case GL_FLOAT_VEC3:
			glUniform3f(m_uniformDataLoc, *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1), *(reinterpret_cast<float*>(m_pUniformData) + 2));
			if (m_logForThis) { GameLogger::Log(MessageType::ConsoleOnly, "Passed (%.3f, %.3f, %.3f) to uniform [%d]\n", *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1), *(reinterpret_cast<float*>(m_pUniformData) + 2), m_uniformDataLoc); }
			break;

		case GL_FLOAT:
			glUniform1f(m_uniformDataLoc, *reinterpret_cast<float*>(m_pUniformData));
			break;

		case GL_INT:
			glUniform1i(m_uniformDataLoc, *reinterpret_cast<int*>(m_pUniformData));
			if (m_logForThis) { GameLogger::Log(MessageType::ConsoleOnly, "Passed %d to uniform [%d]\n", *reinterpret_cast<int*>(m_pUniformData), m_uniformDataLoc); }
			break;

		case GL_FLOAT_VEC4:
			glUniform4f(m_uniformDataLoc, *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1), *(reinterpret_cast<float*>(m_pUniformData) + 2), *(reinterpret_cast<float*>(m_pUniformData) + 3));
			if (m_logForThis) { GameLogger::Log(MessageType::ConsoleOnly, "Passed (%.3f, %.3f, %.3f, %.3f) to uniform [%d]\n", *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1), *(reinterpret_cast<float*>(m_pUniformData) + 2), *(reinterpret_cast<float*>(m_pUniformData) + 3), m_uniformDataLoc); }
			break;

		case GL_FLOAT_VEC2:
			if (m_logForThis) { GameLogger::Log(MessageType::ConsoleOnly, "Passed (%.3f, %.3f) to uniform [%d]\n", *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1), m_uniformDataLoc); }
			glUniform2f(m_uniformDataLoc, *reinterpret_cast<float*>(m_pUniformData), *(reinterpret_cast<float*>(m_pUniformData) + 1));
			break;

		case GL_VERTEX_SHADER:
		case GL_GEOMETRY_SHADER:
		case GL_FRAGMENT_SHADER:
			glUniformSubroutinesuiv(m_uniformType, m_uniformDataLoc, reinterpret_cast<GLuint*>(m_pUniformData)); // TODO: IT WORKS BUT ITS NOT READABLE (STUFF BADLY NAMED FOR THIS CASE WHICH IS UGLY ANYWAY)... REFACTOR!!!
			break;

		case GL_TEXTURE0:
		case GL_TEXTURE1:
		case GL_TEXTURE2:
		case GL_TEXTURE3:
		case GL_TEXTURE4:
		case GL_TEXTURE5: // TODO: IF NEED SUPPORT FOR MORE TEXTURES, KEEP ADDING
			glActiveTexture(m_uniformType);
			glBindTexture(GL_TEXTURE_2D, *reinterpret_cast<int*>(m_pUniformData));
			glUniform1i(m_uniformDataLoc, (m_uniformType - GL_TEXTURE0));
			if (m_logForThis) { GameLogger::Log(MessageType::ConsoleOnly, "Passed (%d) to uniform [%d]\n", *reinterpret_cast<int*>(m_pUniformData), m_uniformDataLoc); }
			break;

		default:
			GameLogger::Log(MessageType::cError, "Unknown uniform data type [%d]!\n", m_uniformType);
			return false;
		}

		return true;
	}

	void ** UniformData::GetUniformDataPtrPtr()
	{
		return &m_pUniformData;
	}

	int UniformData::GetUniformDataLoc() const
	{
		return m_uniformDataLoc;
	}
}


