#ifndef OBJCONVERTER_H
#define OBJCONVERTER_H

#include <fstream>
#include "Mesh.h"
#include "BinaryWriter.h"

// Justin Furtado
// 7/23/2016
// ObjConverter.h
// Reads in obj files and converts them to a custom binary format for optimized loading into  the game engine

class ObjConverter
{
public:
	bool Initialize();
	bool Shutdown();
	bool ProcessFile();

private:
	bool ReadFileIntoMesh();
	bool AnalyzeFile();
	bool WriteFileOutToCustomFormat();
	bool ProcessLine(const char *const line, int lineNumber);
	bool OpenFile();
	bool CloseFile();
	bool AddVertex(const char *const line, int lineNumber);
	bool AddVertexPosition(const char *const line, int lineNumber);
	bool AddVertexTexture(const char *const line, int lineNumber);
	bool AddVertexNormal(const char *const line, int lineNumber);
	bool SetVertexColors(int numFloats);
	bool AddFaceIndices(const char *const line, int lineNumber);
	bool ProcessIntoVertices();
	bool ReadConfigValues();
	bool CleanUp();

	//data
	static const int MAX_CHARS = 256;
	static const int RGB = 3;
	static const int MIN_VERTICES = 0;
	static const int MAX_VERTICES = 1500000;
	static const int MIN_INDICES = 0;
	static const int MAX_INDICES = 1500000;
	char m_modelMessageBuffer[MAX_CHARS];
	bool m_analyzeOnly;
	char m_modelString[MAX_CHARS];
	char m_inputFile[MAX_CHARS];
	char m_outputFile[MAX_CHARS];
	int m_numVertices;
	bool m_colorRandomized;
	bool m_errorsAndWarningsOnly;
	float m_color[RGB];
	std::ifstream m_fileStream;
	Engine::Mesh m_meshToWrite;
	Engine::VertexFormat m_meshVertexFormat;
	float *m_pMeshVertexPositions;
	float *m_pMeshVertexColors;
	float *m_pMeshVertexTextureCoords;
	float *m_pMeshVertexNormals;
	int *m_pTempIndices;

	Engine::BinaryWriter m_customMeshWriter;
};

#endif // ifndef OBJCONVERTER_H