#ifndef MESH_H
#define MESH_H

// Justin Furtado
// 7/6/2016
// Mesh.h
// A mesh class

#include "ExportHeader.h"
#include "GL\glew.h"
#include "GameLogger.h"
#include "RenderInfo.h"
#include "VertexFormat.h"
#include "GameLogger.h"

namespace Engine
{
	enum class ENGINE_SHARED IndexSizeInBytes
	{
		Ubyte = 1,
		Ushort = 2,     // two  byte indices
		Uint = 4,     // four byte indices
	};

	// Enum helps avoid magic numbers
	enum class ENGINE_SHARED ModelSize
	{
		Small = 256, // 1-256
		Medium = 65536, // use Ushort for 257-65536 indices
		Large = 65537  // use Uint for more than 65536 indices 
	};

	class ENGINE_SHARED Mesh
	{
	public:
		// ctor/dtor
		Mesh()
			: m_vertexCount(0), m_indexCount(0), m_pVertices(nullptr), m_pIndices(nullptr), m_renderInfo(RenderInfo()), m_meshMode(GL_TRIANGLES), m_indexSize(IndexSizeInBytes::Uint), m_indexed(true), m_shaderProgramID(0), m_vertexFormat(VertexFormat::None), m_isCullingEnabledForObject(true) {}
		Mesh(GLuint vertexCount, GLuint indexCount, void *pVertices, void *pIndices, GLenum meshMode, IndexSizeInBytes indexSize, GLuint shaderProgramID, VertexFormat format, bool cullForObject = true)
			: m_vertexCount(vertexCount), m_indexCount(indexCount), m_pVertices(pVertices), m_pIndices(pIndices), m_renderInfo(RenderInfo()), m_meshMode(meshMode),
			m_indexed(indexCount > 0), m_indexSize(indexSize), m_shaderProgramID(shaderProgramID), m_vertexFormat(format), m_isCullingEnabledForObject(cullForObject) {}
		~Mesh() {}

		// getters
		void *GetVertexPointer() { return m_pVertices; }
		void *GetIndexPointer() { return m_pIndices; }
		GLuint GetIndexCount() { return m_indexCount; }
		GLuint GetVertexCount() { return m_vertexCount; }
		RenderInfo *GetRenderInfoPtr() { return &m_renderInfo; }
		GLenum GetMeshMode() { return m_meshMode; }
		GLboolean IsIndexed() { return m_indexed; }
		GLuint GetShaderProgramID() { return m_shaderProgramID; }
		IndexSizeInBytes GetIndexSize() { return m_indexSize; }
		GLuint GetSizeOfVertex() { return VertexFormatSize(m_vertexFormat); }
		bool IsCullingEnabledForObject() { return m_isCullingEnabledForObject; }
		GLuint GetTextureID() { return m_textureID; }
		GLuint *GetTextureIDPtr() { return &m_textureID; }
		
		void *GetPointerToVertexAt(unsigned int index)
		{
			if (index > m_vertexCount) { GameLogger::Log(MessageType::cWarning, "Tried to GetVertexAt [%d] but it was out of range!\n", index); return nullptr; }
			return reinterpret_cast<char *>(m_pVertices) + (GetSizeOfVertex() * index);
		}

		void *GetNormalAt(unsigned int index)
		{
			if (!(VertexFormat::HasNormal & m_vertexFormat)) { GameLogger::Log(MessageType::cWarning, "Tried to GetNormalAt [%d] but vertex format does not contain normal data!\n", index); return nullptr; }
			
			// PCTN, PCN, PTN - TAKES ADVANTAGE OF NORMALS ALWAYS BEING LAST...
			// if it has normals, the start of the normals is just the vertex plus the size of everything but the normals
			return reinterpret_cast<char *>(GetPointerToVertexAt(index)) + (GetSizeOfVertex() - NORMAL_BYTES);
		}

		// setters
		void SetRenderInfo(RenderInfo *renderInfo)
		{
			if (!renderInfo) { return; }
			m_renderInfo = *renderInfo;
		}

		void SetShaderProgramID(GLint shaderProgramID) { m_shaderProgramID = shaderProgramID; }
		void SetTextureID(GLuint textureID) { m_textureID = textureID; }

		// helpful methods
		GLuint GetVertexSizeInBytes() { return m_vertexCount * GetSizeOfVertex(); }
		GLuint GetIndexSizeInBytes() { return m_indexCount * static_cast<int>(m_indexSize); }
		VertexFormat GetVertexFormat() { return m_vertexFormat; }

		void ConsoleLogVerts()
		{
			for (GLuint i = 0; i < m_vertexCount; ++i)
			{
				GameLogger::Log(MessageType::cDebug, "verts[%u] = (%3.3f, %3.3f, %3.3f)\n", i, *(reinterpret_cast<float*>(m_pVertices) + i), *(reinterpret_cast<float*>(m_pVertices) + i + 1), *(reinterpret_cast<float*>(m_pVertices) + i + 2));
			}
		}

		void ConsoleLogIndices()
		{
			if (!m_indexed) { GameLogger::Log(MessageType::cWarning, "Tried to ConsoleLogIndices() for non-indexed mesh!\n"); return; }
			for (GLuint i = 0; i < m_indexCount; ++i)
			{
				GameLogger::Log(MessageType::cDebug, "indices[%u] = %u\n", i, *(reinterpret_cast<unsigned int*>(m_pIndices) + i));
			}
		}

	private:
		void *m_pVertices;
		void *m_pIndices;
		GLuint m_vertexCount;
		GLuint m_indexCount;
		GLuint m_textureID;
		GLenum m_meshMode;
		RenderInfo m_renderInfo;
		IndexSizeInBytes m_indexSize;
		GLboolean m_indexed;
		VertexFormat m_vertexFormat;
		GLuint m_shaderProgramID;
		bool m_isCullingEnabledForObject;
	};
}

#endif // ifndef MESH_H