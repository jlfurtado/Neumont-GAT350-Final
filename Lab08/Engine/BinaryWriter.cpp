#include "BinaryWriter.h"
#include "GameLogger.h"
#include "Mesh.h"

// Justin Furtado
// 7/18/2016
// BinaryWriter.cpp
// Writes meshes to a custom binary format

namespace Engine
{
	bool BinaryWriter::WriteCustomBinaryFile(const char * const fileName, Mesh * pMesh)
	{
		// no data yet
		int totalBytes = 0;

		// create a binary output stream
		std::ofstream outputStream(fileName, std::ios::binary | std::ios::out);

		// make sure stream is valid
		if (!outputStream)
		{
			GameLogger::Log(MessageType::cError, "Failed to write custom binary file to [%s], output stream not good!\n", fileName);
			return false;
		}

		// seek to begining of file to ensure proper state
		outputStream.seekp(0);

		// write out placeholder value for number of bytes
		WriteInt(outputStream, totalBytes);

		// make a copy of the mesh
		Mesh m = *pMesh;

		// zero-out part of info that we don't care about yet
		memset(m.GetRenderInfoPtr(), 0, sizeof(*m.GetRenderInfoPtr()));
		void * p = m.GetIndexPointer();
		memset(&p, 0, sizeof(m.GetIndexPointer()));
		p = m.GetVertexPointer();
		memset(&p, 0, sizeof(m.GetVertexPointer()));

		// Write out mesh, update bytes
		totalBytes += WriteMeshData(outputStream, &m);

		// Write out vertices, update bytes
		totalBytes += WriteVertexData(outputStream, m.GetVertexCount(), m.GetVertexPointer(), m.GetSizeOfVertex());

		// Write out indices, update bytes
		totalBytes += WriteIndexData(outputStream, m.GetIndexCount(), m.GetIndexPointer(), (int)m.GetIndexSize());

		// seek to begining of file to ensure proper state
		outputStream.seekp(0);

		// write accurate number of bytes
		WriteInt(outputStream, totalBytes);

		// close stream
		outputStream.close();

		// Log success
		GameLogger::Log(MessageType::Process, "Successfully wrote out [%d] bytes to custom binary file [%s]!\n", totalBytes, fileName);
		return true;
	}

	int BinaryWriter::WriteMeshData(std::ofstream & out, Mesh * pMesh)
	{
		// get size of mesh
		int size = sizeof(*pMesh);

		// write out as binary
		out.write(reinterpret_cast<char*>(pMesh), size);

		// return number of bytes written and log
		GameLogger::Log(MessageType::cProcess, "Wrote [%d] bytes for mesh!\n", size);
		return size;
	}

	int BinaryWriter::WriteVertexData(std::ofstream & out, int numVertices, void* pVertices, int vertexSize)
	{
		// get size of mesh
		int size = vertexSize*numVertices;

		// write out as binary
		out.write(reinterpret_cast<char*>(pVertices), size);

		// return number of bytes written and log
		GameLogger::Log(MessageType::cProcess, "Wrote [%d] bytes for vertices!\n", size);
		return size;
	}

	int BinaryWriter::WriteIndexData(std::ofstream & out, int numIndices, void * pIndices, int indexSize)
	{
		// get size of mesh
		int size = indexSize*numIndices;

		// write out as binary
		out.write(reinterpret_cast<char*>(pIndices), size);

		// return number of bytes written and log
		GameLogger::Log(MessageType::cProcess, "Wrote [%d] bytes for indices!\n", size);
		return size;
	}

	int BinaryWriter::WriteInt(std::ofstream& out, int value)
	{
		// get size of mesh
		int size = sizeof(value);

		// write out as binary
		out.write(reinterpret_cast<char*>(&value), size);

		// return number of bytes written and log
		GameLogger::Log(MessageType::cProcess, "Wrote [%d] bytes for int value!\n", size);
		return size;
	}
}