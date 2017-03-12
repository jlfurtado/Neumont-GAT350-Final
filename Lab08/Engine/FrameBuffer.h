#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "ExportHeader.h"
#include "MyGL.h"

// Justin Furtado
// 1/30/2017
// FrameBuffer.h
// Class to hold framebuffer code

namespace Engine
{
	class ENGINE_SHARED FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		//TODO: REFACTOR TO MAKE MORE DYNAMIC!!!
		bool InitializeForTexture(int width, int height);
		bool InitializeForDepth(int width, int height, bool nearest);
		bool InitializeForShadows(int width, int height);
		void Bind();
		void UnBind(int viewX, int viewY, int viewWidth, int viewHeight);
		GLuint * GetTexIdPtr();
		GLuint * GetTexId2Ptr();
		int *GetWidthPtr();
		int GetWidth();
		int GetHeight();

	private:
		bool InitTexture(GLuint *texIdPtr);
		int width;
		int height;
		GLuint renderTextureId;
		GLuint renderTexture2Id;
		GLuint fboHandle;
		GLuint depthBufferId;
	};
}

#endif // ifndef FRAMEBUFFER_H