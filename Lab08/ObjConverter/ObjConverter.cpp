#include "ObjConverter.h"
#include "GameLogger.h"
#include "ConfigReader.h"
#include "StringFuncs.h"
#include "ColorVertex.h" // todo check if needed
#include "Vertex.h"
#include <istream>
#include <sstream>

// Justin Furtado
// 7/23/2016
// ObjConverter.cpp
// Reads in obj files and converts them to a custom binary format for optimized loading into  the game engine

bool ObjConverter::Initialize()
{
	// if unable to read necessary config values indicate failure
	if (!ReadConfigValues())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to initialize! Could not ReadConfigValues!\n");
		return false;
	}

	// indicate success
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter initialized successfully!\n");
	return true;
}

bool ObjConverter::Shutdown()
{
	// indicate success
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter shutdown successfully!\n");
	return true;
}

bool ObjConverter::ProcessFile()
{
	// try to open input file, if it fails, do not proceed - indicate failure
	if (!OpenFile())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ProcessFile()! Failed to open input file [%s]!\n", m_inputFile);
		return false;
	}

	// if analyze mode only analyze
	if (m_analyzeOnly && !AnalyzeFile())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ProcessFile()! Failed to analyze input file [%s]!\n", m_inputFile);
		return false;
	}

	// try to read data from input file into a mesh, if it fails, do not proceed - indicate failure
	if (!m_analyzeOnly && !ReadFileIntoMesh())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ProcessFile()! Failed to read input file [%s] into mesh!\n", m_inputFile);
		return false;
	}
	
	// try to close input file, if it fails, mark a warning and proceed
	if (!CloseFile())
	{
		Engine::GameLogger::Log(Engine::MessageType::cWarning, "ObjConverter failed to close input file [%s]! ObjConverter will continue to ProcessFile()!\n", m_inputFile);
	}

	// try to write data from mesh into custom binary format, if it fails, indicate failure
	if (!m_analyzeOnly && !WriteFileOutToCustomFormat())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ProcessFile()! Failed to write mesh to custom format!\n");
		return false;
	}

	// After writing has finished clean up, no need to clean up if only analyzed because nothing new'd
	if (!m_analyzeOnly && !CleanUp())
	{
		Engine::GameLogger::Log(Engine::MessageType::cWarning, "ObjConverter failed to clean up!\n");
	}

	// success was achieved 
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully processed input file [%s]!\n", m_inputFile);
	return true;
}

const int MAX_LINE_SIZE = 512;
bool ObjConverter::ReadFileIntoMesh()
{
	// create space to place things into mesh properly
	// TODO: remove hard-coded GL_TRIANGLES, shader ID and Vertex format!!!
	int numFloats = m_numVertices*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float);
	m_meshToWrite = Engine::Mesh(m_numVertices, 0, new float[numFloats], nullptr, GL_TRIANGLES, Engine::IndexSizeInBytes::Uint, 0, m_meshVertexFormat);

	// make plenty of space for data to be filled in
	m_pMeshVertexPositions = new float[numFloats] {0.0f};
	m_pMeshVertexColors = new float[numFloats] {0.0f};
	m_pMeshVertexTextureCoords = new float[numFloats] {0.0f};
	m_pMeshVertexNormals = new float[numFloats] {0.0f};
	m_pTempIndices = new int[m_numVertices * 9]{ 0 };

	// create a buffer to hold the line and an integer to store the line number in
	char line[MAX_LINE_SIZE]{ 0 };
	int lineNumber = 0;

	// set colors if colors should be set
	if (m_meshVertexFormat & Engine::VertexFormat::HasColor)
	{
		if (!SetVertexColors(numFloats))
		{
			Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to set colors for mesh that should have colors set!\n");
			return false;
		}
	}

	// loop through each line in the file and process it, storing the recorded values 
	while (m_fileStream.getline(line, MAX_LINE_SIZE))
	{
		// Increment line number
		++lineNumber;

		// if processing fails for any line
		if (!ProcessLine(line, lineNumber))
		{
			// close the file and indicate failure
			CloseFile();
			Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadFileIntoMesh()! Unable to ProcessLine() for line #%d: [%s]!\n", lineNumber, line);
			return false;
		}
	}

	// convert the recorded values into the vertices for a mesh
	if (!ProcessIntoVertices())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadFileIntoMesh()! Read in data then failed to convert to mesh vertices!\n");
		return false;
	}

	// if the end of the file has been reached and no lines failed to process indicate success
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully read file [%s] into a mesh!\n", m_inputFile);
	return true;
}

bool ObjConverter::AnalyzeFile()
{
	// variable to hold vertex count 
	int vertexCount = 0;

	// char array to hold line and int for line number
	char line[MAX_LINE_SIZE]{ 0 };
	int lineNumber = 0;

	// loop through each line of the file
	while (m_fileStream.getline(line, MAX_LINE_SIZE))
	{
		// increment line number
		++lineNumber;

		// increment count
		if (line[0] == 'f') { vertexCount += 3; } //TODO: replace if adding support for non-triangulated meshes
	}

	// output values and indicate success
	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "ObjConverter analyzed file [%s] and found [%d] vertices!\n", m_inputFile, vertexCount);
	return true;
}

bool ObjConverter::WriteFileOutToCustomFormat()
{
	// try to write file, if it fails indicate failure
	if (!m_customMeshWriter.WriteCustomBinaryFile(&m_outputFile[0], &m_meshToWrite))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to output mesh to custom format! BinaryWriter.WriteCustomBinaryFile([%s], [%p]) failed!\n", m_outputFile, &m_meshToWrite);
		return false;
	}

	// indicate success
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully wrote mesh to custom binary format!\n");
	return true;
}

bool ObjConverter::ProcessLine(const char *const line, int lineNumber)
{
	// ignore comments!
	if (line[0] == '#') { return true; }

	// the line to be processed contains vertex data
	if (line[0] == 'v')
	{
		// try to add the vertex and respond accordingly
		if (!AddVertex(line, lineNumber))
		{
			Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to process line #%d: [%s]! AddVertex() failed!\n", lineNumber, line);
			return false;
		}
		
		// indicate success with no duplicate logging
		return true;
	}

	// the line to be processed contains index data
	if (line[0] == 'f')
	{
		// try to add the indices and respond accordingly
		if (!AddFaceIndices(line, lineNumber))
		{
			Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to process line #%d: [%s]! AddFaceIndices() failed!\n", lineNumber, line);
			return false;
		}

		// indicate success with no duplicate logging
		return true;
	}

	// some parts of file are not yet processed
	if (!m_errorsAndWarningsOnly)
	{
		Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "Un-Processed line [%s] detected!\n", line);
	}
	return true;
}

bool ObjConverter::OpenFile()
{
	// open the .obj file
	m_fileStream.open(m_inputFile, std::ios::in);
	
	// if opened stream is bad indicate failure
	if (!m_fileStream)
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to open input file [%s]! File stream is no good!\n", m_inputFile);
		return false;
	}

	// indicate success
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully opened file [%s]!\n", m_inputFile);
	return true;
}

bool ObjConverter::CloseFile()
{
	// close file stream only if it is open
	if (m_fileStream.is_open())
	{
		m_fileStream.close();
	}

	// if file stream is still open after trying to close it indicate failure
	if (m_fileStream.is_open())
	{
		Engine::GameLogger::Log(Engine::MessageType::cWarning, "ObjConverter failed to close input file stream [%s]!\n", m_inputFile);
		return false;
	}

	// indicate success
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully closed input file [%s]!\n", m_inputFile);
	return true;
}

bool ObjConverter::AddVertex(const char *const line, int lineNumber)
{
	// add vertex based on which part of the vertex it is and the vertex format
	if (Engine::ConfigReader::pReader->WhiteSpace(line[1]) && (m_meshVertexFormat & Engine::VertexFormat::HasPosition)) { return AddVertexPosition(line, lineNumber); }
	else if (line[1] == 't' && (m_meshVertexFormat & Engine::VertexFormat::HasTexture)) { return AddVertexTexture(line, lineNumber); }
	else if (line[1] == 'n' && (m_meshVertexFormat & Engine::VertexFormat::HasNormal)) { return AddVertexNormal(line, lineNumber); }
	else { return true; }
}

const int FLOATS_PER_POSITION = 3;
bool ObjConverter::AddVertexPosition(const char * const line, int /*lineNumber*/)
{
	// keep track of the location to add the next vertex
	static int m_nextVertex = 0;

	// line should look like v [float] [float] [float]
	// create std::things to parse the line
	std::string word;
	std::istringstream parse(line);

	// grab the letter v
	parse >> word;

	// grab the first float
	parse >> *(m_nextVertex*FLOATS_PER_POSITION + m_pMeshVertexPositions);

	// grab the second float
	parse >> *(m_nextVertex*FLOATS_PER_POSITION + m_pMeshVertexPositions + 1);

	// grab the third float
	parse >> *(m_nextVertex*FLOATS_PER_POSITION + m_pMeshVertexPositions + 2);

	// increment vertex number
	++m_nextVertex;

	// indicate success
	if (!m_errorsAndWarningsOnly)
	{
		Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully added vertex position #%d (%.3f, %.3f, %.3f) to mesh!\n", m_nextVertex - 1, *(m_pMeshVertexPositions+m_nextVertex*FLOATS_PER_POSITION), *(m_pMeshVertexPositions + m_nextVertex*FLOATS_PER_POSITION + 1), *(m_pMeshVertexPositions + m_nextVertex*FLOATS_PER_POSITION + 2));
	}
	return true;
}

const int FLOATS_PER_TEXTURE = 2;
bool ObjConverter::AddVertexTexture(const char * const line, int /*lineNumber*/)
{
	// keep track of the location to add the next vertex
	static int m_nextVertex = 0;

	// line should look like vt [float] [float]
	// create std::things to parse the line
	std::string word;
	std::istringstream parse(line);

	// grab the letters v and t
	parse >> word;

	// grab the first float
	parse >> *(m_nextVertex*FLOATS_PER_TEXTURE + m_pMeshVertexTextureCoords);

	// grab the second float
	parse >> *(m_nextVertex*FLOATS_PER_TEXTURE + m_pMeshVertexTextureCoords + 1);

	// increment vertex number
	++m_nextVertex;

	// indicate success
	if (!m_errorsAndWarningsOnly)
	{
		Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully added vertex texture #%d (%.3f, %.3f) to mesh!\n", m_nextVertex - 1, *(m_pMeshVertexTextureCoords + m_nextVertex*FLOATS_PER_TEXTURE), *(m_pMeshVertexTextureCoords + m_nextVertex*FLOATS_PER_TEXTURE + 1));
	}
	return true;
}

const int FLOATS_PER_NORMAL = 3;
bool ObjConverter::AddVertexNormal(const char * const line, int /*lineNumber*/)
{
	// keep track of the location to add the next vertex
	static int m_nextVertex = 0;

	// line should look like vn [float] [float] [float]
	// create std::things to parse the line
	std::string word;
	std::istringstream parse(line);

	// grab the letter v
	parse >> word;

	// grab the first float
	parse >> *(m_nextVertex*FLOATS_PER_NORMAL + m_pMeshVertexNormals);

	// grab the second float
	parse >> *(m_nextVertex*FLOATS_PER_NORMAL + m_pMeshVertexNormals + 1);

	// grab the third float
	parse >> *(m_nextVertex*FLOATS_PER_NORMAL + m_pMeshVertexNormals + 2);

	// increment vertex number
	++m_nextVertex;

	// indicate success
	if (!m_errorsAndWarningsOnly)
	{
		Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully added vertex position #%d (%.3f, %.3f, %.3f) to mesh!\n", m_nextVertex - 1, *(m_pMeshVertexNormals + m_nextVertex*FLOATS_PER_NORMAL), *(m_pMeshVertexNormals + m_nextVertex*FLOATS_PER_NORMAL + 1), *(m_pMeshVertexNormals + m_nextVertex*FLOATS_PER_NORMAL + 2));
	}
	return true;
}

bool ObjConverter::SetVertexColors(int numFloats)
{
	for (int i = 0; i < numFloats; i += 3)
	{
		if (!m_colorRandomized)
		{
			// set color to config color
			*(m_pMeshVertexColors + i) = m_color[0];
			*(m_pMeshVertexColors + i + 1) = m_color[1];
			*(m_pMeshVertexColors + i + 2) = m_color[2];
		}
		else
		{
			// randomize colors
			*(m_pMeshVertexColors + i) = (1.0f*rand()) / RAND_MAX;
			*(m_pMeshVertexColors + i + 1) = (1.0f*rand()) / RAND_MAX;
			*(m_pMeshVertexColors + i + 2) = (1.0f*rand()) / RAND_MAX;
		}
	}

	Engine::GameLogger::Log(Engine::MessageType::cProcess, "Successfully set colors for [%d] floats!\n", numFloats);
	return true;
}

bool ObjConverter::AddFaceIndices(const char *const line, int /*lineNumber*/)
{
	// keep track of the next index location
	static int m_nextIndex = 0;

	// std::things to parse line
	std::istringstream parse(line);
	std::string word;

	// eat the f and the space after it
	parse >> word;
	parse.get();

	// array to hold indices for x/x/x x/x/x x/x/x and ints for locations
	int indices[9]; // 1-based
	int type = 0;
	int index = 0;

	// while the line is not yet ended
	while (parse.peek() > 0)
	{
		// if it is a number store the number
		if (parse.peek() >= '0' && parse.peek() <= '9')
		{
			// grab and store number at proper location
			parse >> indices[index * 3 + type];
		}

		// if it is a slash move index
		if (parse.peek() == '/')
		{
			// eat the '/'
			parse.get();

			// indicate we have changed the index
			type++;
		}

		// if it is whitespace eat it and move to the next index
		if (Engine::ConfigReader::pReader->WhiteSpace(static_cast<char>(parse.peek())))
		{
			// eat the whitespace
			parse.get();

			// indicate we have changed index
			index++;
			type = 0;
		}
	}

	// add indices from array of one-based indices to mesh and convert to zero-based indices
	if (m_meshVertexFormat & Engine::VertexFormat::HasPosition) { m_pTempIndices[m_nextIndex++] = indices[0] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasTexture)  { m_pTempIndices[m_nextIndex++] = indices[1] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasNormal)   { m_pTempIndices[m_nextIndex++] = indices[2] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasPosition) { m_pTempIndices[m_nextIndex++] = indices[3] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasTexture)  { m_pTempIndices[m_nextIndex++] = indices[4] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasNormal)   { m_pTempIndices[m_nextIndex++] = indices[5] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasPosition) { m_pTempIndices[m_nextIndex++] = indices[6] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasTexture)  { m_pTempIndices[m_nextIndex++] = indices[7] - 1; }
	if (m_meshVertexFormat & Engine::VertexFormat::HasNormal)   { m_pTempIndices[m_nextIndex++] = indices[8] - 1; }
	
	// indicate success
	if (!m_errorsAndWarningsOnly)
	{
		Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter added face with indices (%d, %d, %d)!\n", m_nextIndex - 1, m_nextIndex - 2, m_nextIndex - 3);
	}

	return true;
}


bool ObjConverter::ProcessIntoVertices()
{
	int indexCount = ((m_meshVertexFormat & Engine::VertexFormat::HasPosition) ? 1 : 0) + ((m_meshVertexFormat & Engine::VertexFormat::HasTexture) ? 1 : 0) + ((m_meshVertexFormat & Engine::VertexFormat::HasNormal) ? 1 : 0);
	for (int i = 0; i < m_numVertices; ++i)
	{
		int offset = 0;
		int ioffset = 0;
		if (m_meshVertexFormat & Engine::VertexFormat::HasPosition)
		{
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 0) = *(m_pMeshVertexPositions + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 0);
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 1) = *(m_pMeshVertexPositions + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 1);
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 2) = *(m_pMeshVertexPositions + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 2);
			offset += FLOATS_PER_POSITION;
			ioffset++;
		}

		if (m_meshVertexFormat & Engine::VertexFormat::HasColor)
		{
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 0) = *(m_pMeshVertexColors + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 0);
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 1) = *(m_pMeshVertexColors + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 1);
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 2) = *(m_pMeshVertexColors + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 2);
			offset += Engine::COLOR_BYTES/sizeof(float);
		}

		if (m_meshVertexFormat & Engine::VertexFormat::HasTexture)
		{
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 0) = *(m_pMeshVertexTextureCoords + 2 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 0);
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 1) = *(m_pMeshVertexTextureCoords + 2 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 1);
			offset += FLOATS_PER_TEXTURE;
			ioffset++;
		}

		if (m_meshVertexFormat & Engine::VertexFormat::HasNormal)
		{
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 0) = *(m_pMeshVertexNormals + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 0);
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 1) = *(m_pMeshVertexNormals + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 1);
			*(reinterpret_cast<float*>(m_meshToWrite.GetVertexPointer()) + ((i)*Engine::VertexFormatSize(m_meshVertexFormat) / sizeof(float)) + offset + 2) = *(m_pMeshVertexNormals + 3 * (*(m_pTempIndices + (i*indexCount) + ioffset)) + 2);
			offset += FLOATS_PER_NORMAL;
			ioffset++;
		}
	}

	return true;
}

bool ObjConverter::ReadConfigValues()
{
	// read in which model to use
	if (!Engine::ConfigReader::pReader->GetStringForKey("ObjConverter.Model", m_modelString))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get string value for key ObjConverter.Model!\n");
		return false;
	}

	// read in whether to analyze
	if (!Engine::ConfigReader::pReader->GetBoolForKey("ObjConverter.Mode.Analyze", m_analyzeOnly))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get boolean value for key ObjConverter.Mode.Analyze!\n");
		return false;
	}

	// read in whether to log
	if (!Engine::ConfigReader::pReader->GetBoolForKey("ObjConverter.ErrorsAndWarningsOnly", m_errorsAndWarningsOnly))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get boolean value for ObjConverter.ErrorsAndWarningsOnly!\n");
		return false;
	}

	// read in input file name for specific model being used
	Engine::StringFuncs::StringConcatIntoBuffer(m_modelString, "InputFileName", ".", m_modelMessageBuffer, MAX_CHARS);
	if (!Engine::ConfigReader::pReader->GetStringForKey(&m_modelMessageBuffer[0], m_inputFile))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get string value for key [%s]!\n", m_modelMessageBuffer);
		return false;
	}

	// read in output file name for specific model being used
	Engine::StringFuncs::StringConcatIntoBuffer(m_modelString, "OutputFileName", ".", m_modelMessageBuffer, MAX_CHARS);
	if (!Engine::ConfigReader::pReader->GetStringForKey(&m_modelMessageBuffer[0], m_outputFile))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get string value for key [%s]!\n", m_modelMessageBuffer);
		return false;
	}

	// read in num vertices for specific model being used
	Engine::StringFuncs::StringConcatIntoBuffer(m_modelString, "NumVertices", ".", m_modelMessageBuffer, MAX_CHARS);
	if (!Engine::ConfigReader::pReader->GetClampedIntForKey(&m_modelMessageBuffer[0], m_numVertices, MIN_VERTICES, MAX_VERTICES))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get int value for key [%s]!\n", m_modelMessageBuffer);
		return false;
	}

	// read in vertex format
	Engine::StringFuncs::StringConcatIntoBuffer(m_modelString, "Format", ".", m_modelMessageBuffer, MAX_CHARS);
	char buffer[4];
	if (!Engine::ConfigReader::pReader->GetStringForKey(&m_modelMessageBuffer[0], &buffer[0]))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get string value for key [%s]!\n", m_modelMessageBuffer);
		return false;
	}

	// grab vertex format from string
	m_meshVertexFormat = Engine::VertexFormatFromString(&buffer[0]);
	
	// validate value
	if (m_meshVertexFormat == Engine::VertexFormat::None)
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Invalid value for mesh vertex format!\n");
		return false;
	}

	// only read in color if format indicates mesh should be colored
	if (m_meshVertexFormat & Engine::VertexFormat::HasColor)
	{
		// read in color for specific model being used
		char colorString[MAX_CHARS];
		Engine::StringFuncs::StringConcatIntoBuffer(m_modelString, "Color", ".", m_modelMessageBuffer, MAX_CHARS);
		if (!Engine::ConfigReader::pReader->GetStringForKey(&m_modelMessageBuffer[0], colorString))
		{
			Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get string value for key [%s]!\n", m_modelMessageBuffer);
			return false;
		}

		// if set to "Random" or "random" color is randomized
		m_colorRandomized = Engine::StringFuncs::StringsAreEqual(colorString, "Random") || Engine::StringFuncs::StringsAreEqual(colorString, "random");

		// if color is not randomized read in a color
		if (!m_colorRandomized)
		{
			if (!Engine::ConfigReader::pReader->GetClampedFloatsForKey(m_modelMessageBuffer, RGB, &m_color[0], 0.0f, 1.0f))
			{
				Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "ObjConverter failed to ReadConfigValues! Failed to get float values for key [%s]!\n", m_modelMessageBuffer);
				return false;
			}
		}
	}
	
	// indicate success
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully read in all relevant config values!\n");
	return true;
}

bool ObjConverter::CleanUp()
{
	// clean up after new'd vertices and indices
	delete[] m_pMeshVertexPositions;
	delete[] m_pMeshVertexColors;
	delete[] m_pMeshVertexTextureCoords;
	delete[] m_pMeshVertexNormals;
	delete[] m_meshToWrite.GetVertexPointer();
	delete[] m_pTempIndices;
	
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "ObjConverter successfully cleaned up!\n");
	return true;
}
