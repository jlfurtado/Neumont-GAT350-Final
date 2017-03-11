#ifndef BITMAPLOADER_H
#define BITMAPLOADER_H

#include "ExportHeader.h"
#include "GL\glew.h"

// Justin Furtado
// 8/2/2016
// BitmapLoader.h
// Loads bitmap files!

namespace Engine
{
	class ENGINE_SHARED BitmapLoader
	{
	public:
		static bool Initialize();
		static bool Shutdown();
		static GLuint LoadTexture(const char *const relativePath);
		static GLuint SetupWhitePixel();
	};
}

#endif // ifndef BITMAPLOADER_H