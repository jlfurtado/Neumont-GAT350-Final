#ifndef BINARYWRITER_H
#define BINARYWRITER_H

// Justin Furtado
// 7/18/2016
// BinaryWriter.cpp
// Writes meshes to a custom binary format

#include "ExportHeader.h"
#include "GL\glew.h"
#include <fstream>

namespace Engine
{
	class Mesh;

	class ENGINE_SHARED BinaryWriter
	{
	public:
		bool WriteCustomBinaryFile(const char *const fileName, Mesh *pMesh);

	private:
		int WriteMeshData(std::ofstream& out, Mesh *pMesh);
		int WriteVertexData(std::ofstream& out, int numVertices, void *pVertices, int vertexSize);
		int WriteIndexData(std::ofstream& out, int numIndices, void *pIndices, int indexSize);
		int WriteInt(std::ofstream& out, int value);
	};
}

#endif // ifndef BINARYWRITER_H