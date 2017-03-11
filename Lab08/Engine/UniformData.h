#ifndef UNIFORMDATA_H
#define UNIFORMDATA_H

// Justin Furtado
// UniformData.h
// 10/28/2016
// Stores data needed to make uniform calls!

#include "GL\glew.h"
#include "Mat4.h"
#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED UniformData
	{
	public:
		UniformData();
		UniformData(GLenum type, void *dataAddress, int dataLoc, bool log = false);
		bool PassUniform();
		void **GetUniformDataPtrPtr();
		int GetUniformDataLoc() const;

	private:
		int m_uniformDataLoc;
		void *m_pUniformData;
		GLenum m_uniformType;
		bool m_logForThis;
	};
}

#endif // ifndef UNIFORMDATA_H