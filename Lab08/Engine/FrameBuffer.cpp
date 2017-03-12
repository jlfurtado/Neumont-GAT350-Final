#include "FrameBuffer.h"
#include "MyGL.h"

// Justin Furtado
// 1/30/2017
// FrameBuffer.h
// Class to hold framebuffer code

namespace Engine
{
	FrameBuffer::FrameBuffer() 
		: width(0), height(0), depthBufferId(0), fboHandle(0), renderTextureId(0)
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	const GLsizei numTextureLevels = 1;
	const GLenum  internalFormatImage = GL_RGBA8;
	const GLint   xOffset = 0; // offsets into texture
	const GLint   yOffset = 0;
	const GLint   mipMapLevel = 0;
	const GLuint  unbindFbo = 0; // 0 unbinds fbo
	bool FrameBuffer::InitializeForTexture(int w, int h)
	{
		width = w;
		height = h;

		// make the texture
		glGenTextures(1, &renderTextureId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture GenTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Texture in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// bind the texture we just created
		glBindTexture(GL_TEXTURE_2D, renderTextureId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture BindTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Bind Texture in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// send some data about the texture to opengl
		glTexStorage2D(GL_TEXTURE_2D, numTextureLevels, internalFormatImage, width, height);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture TexStorage2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexStorage2D in failedFrameBufffer InitializeForTexture!\n");
			return false;
		}

		// set some parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture TexParameteri Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexParameteri failed in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Create framebuffer object
		glGenFramebuffers(1, &fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture GenFramebuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Framebuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture BindFramebuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to BindFramebuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Bind the texture to the fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureId, mipMapLevel);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture FramebufferTexture2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Hook framebuffer to texture in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Create the depth buffer and bind it
		glGenRenderbuffers(1, &depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture GenRenderbuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Renderbuffers in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// bind the render buffer
		glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture BindRenderbuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Bind Renderbuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// send data to opengl
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture RenderbufferStorage Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to send info about renderbuffer to opengl in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Bind the depth buffer to the FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture FramebufferRenderbuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to hook up the framebuffer to the renderbuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Set the target for the fragment shader outputs
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture DrawBuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to set target for fragment shader outputs in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			GameLogger::Log(MessageType::cError, "FrameBuffer InitializeForTexture Failed! Framebuffer status is not complete!\n");
			return false;
		}

		// Unbind FBO and revert to default
		glBindFramebuffer(GL_FRAMEBUFFER, unbindFbo);
		return true;
	}

	const GLenum  internalFormatDepth = GL_DEPTH_COMPONENT24;
	bool FrameBuffer::InitializeForDepth(int w, int h, bool nearest)
	{
		// store the width and height
		width = w;
		height = h;

		// set stuff outside the border to this value to make everything fully lit for shadow maps
		GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
		
		// make the texture
		glGenTextures(1, &renderTextureId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth GenTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Texture in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, renderTextureId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth BindTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Bind Texture in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		// send data about texture to opengl
		glTexStorage2D(GL_TEXTURE_2D, numTextureLevels, internalFormatDepth, width, height);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth TexStorage2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexStorage2D failed in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		// set parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth TexParameter Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexParameter failed in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		// Create and bind the new fbo
		glGenFramebuffers(1, &fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth GenFramebuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "GenFramebuffer failed in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth BindFramebuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "BindFRamebuffer failed in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		// Bind the texture to the fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderTextureId, mipMapLevel);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth FramebufferTexture2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "FramebufferTexture2D failed in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		// Set the target for the fragment shader outputs
		GLenum drawBuffers[] = { GL_NONE };
		glDrawBuffers(1, drawBuffers);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForDepth DrawBuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "DrawBuffers failed in FrameBufffer InitializeForDepth!\n");
			return false;
		}

		// Check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			GameLogger::Log(MessageType::cError, "FrameBuffer InitializeForDepth Failed! Framebuffer status is not complete!\n");
			return false;
		}

		// Unbind FBO and revert to default
		glBindFramebuffer(GL_FRAMEBUFFER, unbindFbo);
		return true;
	}

	bool FrameBuffer::InitializeForShadows(int w, int h)
	{
		width = w;
		height = h;

		// at least its kinda refactored
		InitTexture(&renderTextureId);
		InitTexture(&renderTexture2Id);

		// Create framebuffer object
		glGenFramebuffers(1, &fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture GenFramebuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Framebuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture BindFramebuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to BindFramebuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Bind the texture to the fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureId, mipMapLevel);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture FramebufferTexture2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Hook framebuffer to texture in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// i'll fix this later
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderTexture2Id, mipMapLevel);

		if (MyGL::TestForError(MessageType::cError, "FrameBuffer Initialize NEWSTUFF Errors"))
		{
			GameLogger::Log(MessageType::cError, "FrameBuffer init failed cuz new stuff\n");
			return false;
		}

		// Create the depth buffer and bind it
		glGenRenderbuffers(1, &depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture GenRenderbuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Renderbuffers in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// bind the render buffer
		glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture BindRenderbuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Bind Renderbuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// send data to opengl
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture RenderbufferStorage Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to send info about renderbuffer to opengl in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Bind the depth buffer to the FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture FramebufferRenderbuffer Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to hook up the framebuffer to the renderbuffer in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Set the target for the fragment shader outputs
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		const GLsizei numDrawBuffers = sizeof(drawBuffers) / sizeof(drawBuffers[0]);
		glDrawBuffers(numDrawBuffers, drawBuffers);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture DrawBuffers Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to set target for fragment shader outputs in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// Check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			GameLogger::Log(MessageType::cError, "FrameBuffer InitializeForTexture Failed! Framebuffer status is not complete!\n");
			return false;
		}

		// Unbind FBO and revert to default
		glBindFramebuffer(GL_FRAMEBUFFER, unbindFbo);
		return true;
	}
	
	void FrameBuffer::Bind()
	{
		// Bind to texture's FBO
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
		glViewport(xOffset, yOffset, width, height);

		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	void FrameBuffer::UnBind(int viewX, int viewY, int viewWidth, int viewHeight)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, unbindFbo);
		glViewport(viewX, viewY, viewWidth, viewHeight);
	}

	GLuint * FrameBuffer::GetTexIdPtr()
	{
		return &renderTextureId;
	}

	GLuint * FrameBuffer::GetTexId2Ptr()
	{
		return &renderTexture2Id;
	}

	int * FrameBuffer::GetWidthPtr()
	{
		return &width;
	}

	int FrameBuffer::GetWidth()
	{
		return width;
	}

	int FrameBuffer::GetHeight()
	{
		return height;
	}

	bool FrameBuffer::InitTexture(GLuint *texIdPtr)
	{
		// make the texture
		glGenTextures(1, texIdPtr);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture GenTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Generate Texture in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// bind the texture we just created
		glBindTexture(GL_TEXTURE_2D, *texIdPtr);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture BindTexture Errors"))
		{
			GameLogger::Log(MessageType::cError, "Failed to Bind Texture in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		// send some data about the texture to opengl
		glTexStorage2D(GL_TEXTURE_2D, numTextureLevels, internalFormatImage, width, height);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture TexStorage2D Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexStorage2D in failedFrameBufffer InitializeForTexture!\n");
			return false;
		}

		// set some parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "FrameBuffer InitializeForTexture TexParameteri Errors"))
		{
			GameLogger::Log(MessageType::cError, "TexParameteri failed in FrameBufffer InitializeForTexture!\n");
			return false;
		}

		return true;
	}
}
