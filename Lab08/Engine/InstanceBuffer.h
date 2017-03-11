#ifndef INSTANCEBUFFER_H
#define INSTANCEBUFFER_H

// Justin Furtado
// 3/1/2017
// InstanceBuffer.h
// Knows how to tell OpenGL about data to be used for instance buffers

#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED InstanceBuffer
	{
	public:
		bool Initialize(void *pData, unsigned stride, unsigned count, unsigned numFloats);
		bool Shutdown();
		bool SetupAttrib(int attribIndex);
		unsigned GetCount();

	private:
		unsigned m_bufferID;
		unsigned m_stride;
		unsigned m_numFloats;
		unsigned m_count;
	};
}

#endif // ifndef INSTANCEBUFFER_H