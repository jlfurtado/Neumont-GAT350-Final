#include "InstanceBuffer.h"
#include "MyGL.h"
#include "GameLogger.h"
#include "Vec4.h"

// Justin Furtado
// 3/1/2017
// InstanceBuffer.h
// Knows how to tell OpenGL about data to be used for instance buffers

namespace Engine
{

	bool InstanceBuffer::Initialize(void *pData, unsigned stride, unsigned count, unsigned numFloats)
	{
		m_stride = stride;
		m_count = count;
		m_numFloats = numFloats;

		int bufferSize = m_stride * m_count;

		// Generate the buffer and check for errors
		glGenBuffers(1, &m_bufferID);
		if (MyGL::TestForError(MessageType::cFatal_Error, "InstanceBuffer Initialize Gen Buffer Errors") || !m_bufferID)
		{
			GameLogger::Log(MessageType::cFatal_Error, "InstanceBuffer was unable to generate a buffer!\n");
			return false;
		}

		// bind the vertex buffer and check for errors
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		if (MyGL::TestForError(MessageType::cFatal_Error, "InstanceBuffer Initialize Bind Buffer Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "InstanceBuffer was unable to bind the buffer with id [%d]!\n", m_bufferID);
			return false;
		}

		// initialize the vertex buffer data and check for errors
		glBufferData(GL_ARRAY_BUFFER, bufferSize, pData, GL_STATIC_DRAW);
		if (MyGL::TestForError(MessageType::cFatal_Error, "InstanceBuffer Initialize Buffer Data Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "InstanceBuffer was unable to create empty buffer of size [%d] bytes!\n", bufferSize);
			return false;
		}

		GameLogger::Log(MessageType::Process, "InstanceBuffer initialized successfully");
		return true;
	}

	unsigned InstanceBuffer::GetCount() { return m_count; }

	bool InstanceBuffer::Shutdown()
	{
		GameLogger::Log(MessageType::Process, "InstanceBuffer shutdown successfully");
		return true;
	}

	bool InstanceBuffer::SetupAttrib(int attribIndex)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

		if (m_numFloats < 16)
		{
			glEnableVertexAttribArray(attribIndex);
			glVertexAttribPointer(attribIndex, m_numFloats, GL_FLOAT, GL_FALSE, m_stride, (GLvoid*)0);
			glVertexAttribDivisor(attribIndex, 1);
		}
		else
		{
			// Vertex Attributes
			GLsizei vec4Size = 4*sizeof(float);
			glEnableVertexAttribArray(attribIndex+0);
			glVertexAttribPointer(attribIndex + 0, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
			glEnableVertexAttribArray(attribIndex+1);
			glVertexAttribPointer(attribIndex + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
			glEnableVertexAttribArray(attribIndex+2);
			glVertexAttribPointer(attribIndex + 2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
			glEnableVertexAttribArray(attribIndex+3);
			glVertexAttribPointer(attribIndex + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

			glVertexAttribDivisor(attribIndex + 0, 1);
			glVertexAttribDivisor(attribIndex + 1, 1);
			glVertexAttribDivisor(attribIndex + 2, 1);
			glVertexAttribDivisor(attribIndex + 3, 1);
		}

		glBindVertexArray(0);


		return MyGL::TestForError(MessageType::cError, "InstanceBufferSetupAttrib errors!");

	}
}