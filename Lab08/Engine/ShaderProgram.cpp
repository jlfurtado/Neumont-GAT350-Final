#include "ShaderProgram.h"
#include "ShaderProgram.h"
#include "GameLogger.h"
#include "MyGL.h"
#include "MyFiles.h"
#include "StringFuncs.h"

// Justin Furtado
// 6/26/2016
// ShaderProgram.h
// Wrapper class for an OpenGL Shader Program

namespace Engine
{
	bool ShaderProgram::Initialize()
	{
		// no errors yet
		bool good = true;

		// make tolerant of multiple calls
		if (!m_deleted)
		{
			Shutdown();
		}

		// create program
		m_id = glCreateProgram();

		// glCreateProgram returns 0 if an error occurs
		if (!m_id)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Unable to create Shader Program! glCreateProgram() returned 0!\n");
			good = false;
		}

		// testForError returns true if at least one error occurred
		if (MyGL::TestForError(MessageType::cFatal_Error, "Shader Program Initialize errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Unable to create Shader Program, TestForError found GL Errors!\n");
			good = false;
		}

		// log success if no error
		if (good) { GameLogger::Log(MessageType::Process, "Initialize() succeeded for ShaderProgram with id [%u]!\n", m_id); m_deleted = GL_FALSE; }

		// init data
		for (int i = 0; i < maxShaders; ++i)
		{
			m_shaderDeleted[i] = GL_TRUE; // mark shaders as usable
		}

		// return whether or not erors occurred
		return good;
	}

	bool ShaderProgram::Shutdown()
	{
		// no errors yet
		bool good = true;

		// delete shaders and program, check for errors
		if (!DeleteShaders()) { GameLogger::Log(MessageType::cFatal_Error, "DeleteShaders() failed for ShaderProgram with id [%u]!\n", m_id); good = false; }
		if (!DeleteProgram()) { GameLogger::Log(MessageType::cFatal_Error, "DeleteProgram() failed for ShaderProgram with id [%u]!\n", m_id); good = false; }

		// if no errors log success message
		if (good) { GameLogger::Log(MessageType::Process, "ShutDown() succeeded for ShaderProgram with id [%u]!\n", m_id); }

		// return whether or not errors occurred
		return good;
	}

	bool ShaderProgram::AddVertexShader(const char * const filename)
	{
		return AddShader(filename, GL_VERTEX_SHADER);
	}

	bool ShaderProgram::AddGeometryShader(const char * const fileName)
	{
		return AddShader(fileName, GL_GEOMETRY_SHADER);
	}

	bool ShaderProgram::AddFragmentShader(const char * const filename)
	{
		return AddShader(filename, GL_FRAGMENT_SHADER);
	}

	bool ShaderProgram::AddShader(const char * const filename, GLenum shaderType)
	{
		// No errors yet!
		bool good = true;

		// =============================
		// Create the shader
		// =============================

		int shaderId = glCreateShader(shaderType);

		// glCreateShader returns 0 if an error occurred
		if (!shaderId)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Coult not add shader [%s] of type [%d], glCreateShader(%d) returned 0!\n", filename, static_cast<int>(shaderType), static_cast<int>(shaderType));
			good = false; // we want the test for error messages too
		}

		if (MyGL::TestForError(MessageType::cFatal_Error, "Add Shader errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not add shader [%s] of type [%d], TestForError() found problems from glCreateShader()!\n", filename, static_cast<int>(shaderType));
			good = false;
		}

		// no need to proceed if unable to create shader...
		if (!good) { return false; }

		// =============================
		// Give the shader source
		// =============================

		char *source = MyFiles::ReadFileIntoString(filename);

		// ReadFileIntoString returns nullptr if an error occurs
		if (source == nullptr)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not add shader [%s] of type [%d], unable to read file [%s] into string!\n", filename, static_cast<int>(shaderType), filename);
			return false; // cannot proceed if unable to read file into string...
		}

		glShaderSource(shaderId, 1, &source, nullptr);

		// no memory leak!!!
		delete[] source;

		if (MyGL::TestForError(MessageType::cFatal_Error, "Add shader errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not add shader [%s] of type [%d], TestForError() found problems from glShaderSource()!\n", filename, static_cast<int>(shaderType));

			glDeleteShader(shaderId);

			// test for errors in deleting it
			if (MyGL::TestForError(MessageType::cError, "Error deleting shaders"))
			{
				GameLogger::Log(MessageType::cError, "Errors present when deleting shader [%d]!\n", shaderId);
			}

			return false; // cannot proceed if unable to give openGL the source for the shader
		}

		// =============================
		// Compile the shader
		// =============================

		glCompileShader(shaderId);

		if (MyGL::TestForError(MessageType::cFatal_Error, "Add shader errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not add shader [%s] of type [%d], TestForError() found problems from glCompileShader()!\n", filename, static_cast<int>(shaderType));
			good = false; // want compile error messages too
		}

		// returns whether or not compile was successful
		if (!CheckShaderCompileStatus(shaderId))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not add shader [%s] of type [%d], shader failed to compile!\n", filename, static_cast<int>(shaderType));
			good = false;
		}

		// cannot proceed if shader does not compile, delete shader and return false
		if (!good)
		{
			glDeleteShader(shaderId);

			// test for errors in deleting it
			if (MyGL::TestForError(MessageType::cError, "Error deleting shaders"))
			{
				GameLogger::Log(MessageType::cError, "Errors present when deleting shader [%d]!\n", shaderId);
			}

			return false;
		}

		// =============================
		// Save Shader Info
		// =============================

		if (!SaveShaderInfo(filename, shaderType, shaderId))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not add shader [%s] of type [%d], unable to save shader info!\n", filename, static_cast<int>(shaderType));

			glDeleteShader(shaderId);

			// test for errors in deleting it
			if (MyGL::TestForError(MessageType::cError, "Error deleting shaders"))
			{
				GameLogger::Log(MessageType::cError, "Errors present when deleting shader [%d]!\n", shaderId);
			}

			return false; // Don't continue if unable to save shader info
		}

		// =============================
		//  Attach shader to program
		// =============================

		glAttachShader(m_id, shaderId);

		if (MyGL::TestForError(MessageType::cFatal_Error, "Add Shader Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not add shader [%s] of type [%d], unable to attach shader to program [%u]!\n", filename, static_cast<int>(shaderType), m_id);

			glDeleteShader(shaderId);

			// test for errors in deleting it
			if (MyGL::TestForError(MessageType::cError, "Error deleting shaders"))
			{
				GameLogger::Log(MessageType::cError, "Errors present when deleting shader [%d]!\n", shaderId);
			}

			// TODO: un-save shader info...
			return false; // Coult not attach shader to program, delete it, remove info, and return false
		}

		GameLogger::Log(MessageType::Process, "AddShader(%s, %d) succeeded for ShaderProgram [%d]!\n", filename, static_cast<int>(shaderType), m_id);
		return true;
	}

	bool ShaderProgram::CheckShaderCompileStatus(GLuint shaderID)
	{
		// no errors yet
		bool good = true;

		// check if it compiled
		GLint shaderCompiled;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);

		// check for errors in checking if it compiled
		if (MyGL::TestForError(MessageType::cFatal_Error, "Shader Compilation Detection Errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to check compilation status for shader [%d]!\n", shaderID);
			return false;
		}

		// check compilation return value
		if (shaderCompiled != GL_TRUE)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Shader [%d] failed to compile!\n", shaderID);
			good = false;
		}

		// if something went wrong
		if (!good)
		{
			// get length of log
			GLint logLength;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

			// check for errors in getting length of log
			if (MyGL::TestForError(MessageType::cFatal_Error, "Info Log Errors"))
			{
				GameLogger::Log(MessageType::cFatal_Error, "Could not obtain info log length for shader [%d], TestForError found errors from glGetShaderiv()!\n", shaderID);
				return false; // Can't print message without its length
			}

			// create buffer to hold log
			GLchar *errorMessageBuffer = new GLchar[logLength];

			// display the error message
			ShowShaderLogInfo(shaderID, errorMessageBuffer, logLength);

			// no memory leak!
			delete[] errorMessageBuffer;
			return false;
		}

		GameLogger::Log(MessageType::Process, "Shader [%d] compiled successfully!\n", shaderID);
		return true;
	}

	bool ShaderProgram::IsLinked() const
	{
		return m_isLinked;
	}

	bool ShaderProgram::CheckProgramStatus(GLenum pname)
	{
		// get program status
		GLint programStatus;
		glGetProgramiv(m_id, pname, &programStatus);

		// check for errors in getting the status
		if (MyGL::TestForError(MessageType::cError, "Errors in getting program status"))
		{
			GameLogger::Log(MessageType::cError, "Could not get program status for program [%d], errors in getting program status!\n", m_id);
			return false;
		}

		// Program status bad
		if (programStatus == GL_FALSE)
		{
			// get log length
			GLint logLength;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logLength);

			// check for errors in getting log length
			if (MyGL::TestForError(MessageType::cError, "Info log length obtaining errors"))
			{
				GameLogger::Log(MessageType::cError, "Cannot get info log length for program [%d]!\n", m_id);
				return false;
			}

			// create buffer for log
			GLchar *infoLogBuffer = new GLchar[logLength];

			// log the info
			ShowProgramLogInfo(infoLogBuffer, logLength, pname);

			// no memory leak!
			delete[] infoLogBuffer;
			return false;
		}

		// success
		GameLogger::Log(MessageType::Info, "Program [%d] status for [%u] good!\n", m_id, static_cast<unsigned int>(pname));
		return true;
	}

	bool ShaderProgram::CheckProgramStatus()
	{
		// check link status
		if (!CheckProgramStatus(GL_LINK_STATUS))
		{
			GameLogger::Log(MessageType::Info, "Shader Program [%u] link status bad!\n", m_id);
			return false;
		}

		// validate
		glValidateProgram(m_id);

		// check for errors
		if (MyGL::TestForError(MessageType::cError, "Errors in program validation"))
		{
			GameLogger::Log(MessageType::cError, "Program [%d] validation errors present!\n", m_id);
			return false;
		}

		// check validation
		if (!CheckProgramStatus(GL_VALIDATE_STATUS))
		{
			GameLogger::Log(MessageType::cError, "Program [%d] validation errors present!\n", m_id);
			return false;
		}

		// all good
		GameLogger::Log(MessageType::Info, "Program status is good!\n");
		return true;
	}

	bool ShaderProgram::LinkProgram()
	{
		// no errors yet!
		bool good = true;

		// Link the program
		glLinkProgram(m_id);

		// linking errors present
		if (MyGL::TestForError(MessageType::cFatal_Error, "Shader Program linkage errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Linking of program [%d] failed!\n", m_id);
			good = false;
		}

		// If program is no-good
		if (!CheckProgramStatus())
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not link program [%d], Program status was bad!\n", m_id);
			good = false;
		}

		// If program cannot be linked clean up and return false to indicate failure
		if (!good) { DeleteShaders(); DeleteProgram(); return false; }

		// try to parse shader
		if (!ParseShader()) { GameLogger::Log(MessageType::cFatal_Error, "Failed to parse shader program [%d]!\n"); return false; }

		// set flag
		m_isLinked = true;
		GameLogger::Log(MessageType::Process, "Successfully linked shader program [%d]!\n", m_id);
		return true;
	}

	bool ShaderProgram::UseProgram()
	{
		// link succeeded, use the program!
		glUseProgram(m_id);

		if (MyGL::TestForError(MessageType::cFatal_Error, "Unable to use linked program errors"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Unable to use the linked program [%d]!\n", m_id);
			return false;
		}

		// success!!!
		// This log gets spammy... GameLogger::Log(MessageType::Process, "Successfully used shader program [%d]!\n", m_id);
		return true;
	}

	bool ShaderProgram::LinkAndUseProgram()
	{
		return LinkProgram() && UseProgram();
	}

	GLint ShaderProgram::GetUniformLocation(const GLchar * name)
	{
		// get location
		GLint id = glGetUniformLocation(m_id, name);

		// test for errors in getting location
		if (MyGL::TestForError(MessageType::cError, "Errors in getting uniform location"))
		{
			GameLogger::Log(MessageType::cError, "Errors detected when getting uniform location for [%s]!\n", name);
		}

		// check validity of id
		if (id < 0)
		{
			GameLogger::Log(MessageType::cError, "Invalid location returned for uniform [%s]!\n", name);
		}

		return id;
	}

	GLint ShaderProgram::GetAttribLocation(const GLchar * name)
	{
		// get location
		GLint id = glGetAttribLocation(m_id, name);

		// test for errors in getting location
		if (MyGL::TestForError(MessageType::cError, "Errors in getting attrib location"))
		{
			GameLogger::Log(MessageType::cError, "Errors detected when getting attrib location for [%s]!\n", name);
		}

		// check validity of id
		if (id < 0)
		{
			GameLogger::Log(MessageType::cError, "Invalid location returned for attrib [%s]!\n", name);
		}

		return id;
	}

	GLint ShaderProgram::GetSubroutineIndex(GLenum shaderType, const GLchar * name)
	{
		// ask opengl for the location
		GLint id = glGetSubroutineIndex(m_id, shaderType, name);

		// see if opengl yelled at us
		if (MyGL::TestForError(MessageType::cError, "Errors in getting subroutine index"))
		{
			GameLogger::Log(MessageType::cError, "Errors detected when getting subroutine index fpr [%s]!\n", name);
		}
		
		// check validity of id
		if (id < 0)
		{
			GameLogger::Log(MessageType::cError, "Invalid subroutine index returned for [%s]!\n", name);
		}

		return id;
	}

	bool ShaderProgram::ParseShader()
	{
		return DisplayShaderInfoHeader() && ShowActiveAttributes() && ShowActiveUniforms(true); // TODO: CHECK BOOLEAN
	}

	// displays information about the active attributes for the shader to the console
	bool ShaderProgram::ShowActiveAttributes()
	{
		// get the number of attributes
		int numAttributes = GetNumAttributes();

		// loop over them
		for (int i = 0; i < numAttributes; ++i)
		{
			if (!DisplaySingleAttribute(i)) { GameLogger::Log(MessageType::cError, "Failed to display Attribute number [%d] for Shader Program [%d]!\n", i, m_id); return false; }
		}

		// return boolean indicating if error occurred 
		return (numAttributes >= 0);
	}

	// displays information about the active uniforms for the shader to the console
	bool ShaderProgram::ShowActiveUniforms(bool showUniformBlocks)
	{
		// get the number of uniforms
		int numUniforms = GetNumUniforms();

		// loop over them
		for (int i = 0; i < numUniforms; ++i)
		{
			if (!DisplaySingleUniform(i, showUniformBlocks)) { GameLogger::Log(MessageType::cError, "Failed to display Uniform number [%d] for Shader Program [%d]!\n", i, m_id); return false; }
		}

		// return a boolean indicating if error occurred
		return (numUniforms >= 0);
	}

	void ShaderProgram::ShowShaderLogInfo(GLuint shaderID, char * infoBuffer, GLint bufferLen)
	{
		// get the log
		glGetShaderInfoLog(shaderID, bufferLen, nullptr, infoBuffer);

		// test for errors in getting log
		if (MyGL::TestForError(MessageType::cFatal_Error, "Errors in obtaining info log for compilation failure"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to get shader info log for shader [%d]!\n", shaderID);
			return; // can't do anything here without log
		}

		if (bufferLen > GameLogger::MAX_LOG_SIZE)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Cannot write shader info log of size [%d], max log size is [%d]", bufferLen, GameLogger::MAX_LOG_SIZE);
			return; // unable to log because message is too large
		}

		GameLogger::Log(MessageType::cFatal_Error, "------\n%s\n", infoBuffer);
	}

	void ShaderProgram::ShowProgramLogInfo(char * infoBuffer, GLint bufferLen, GLenum pname)
	{
		// get the log
		glGetProgramInfoLog(m_id, bufferLen, nullptr, infoBuffer);

		// test for errors in getting the log
		if (MyGL::TestForError(MessageType::cFatal_Error, "Errors in obtaining info log for program status"))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to get program info log for program [%d]!\n", m_id);
			return; // can't do anything here without log
		}

		if (bufferLen > GameLogger::MAX_LOG_SIZE)
		{
			GameLogger::Log(MessageType::cFatal_Error, "Cannot write program info log of size [%d], max log size is [%d]", bufferLen, GameLogger::MAX_LOG_SIZE);
			return; // unable to log because message is too large
		}

		GameLogger::Log(MessageType::cFatal_Error, "------pname = [%u]\n%s\n", pname, infoBuffer);
	}

	bool ShaderProgram::SaveShaderInfo(const char * const fileName, GLenum shaderType, GLuint shaderId)
	{
		// Iterate through shaders
		for (int i = 0; i < maxShaders; ++i)
		{
			// if a space is free
			if (m_shaderDeleted[i])
			{
				// use it
				m_shaders[i].id = shaderId;
				StringFuncs::StringCopy(fileName, m_shaders[i].name, maxShaderNameLen);
				m_shaders[i].type = shaderType;

				// mark as used
				m_shaderDeleted[i] = GL_FALSE;

				// log and return
				GameLogger::Log(MessageType::Info, "Shader [%s] with id [%d] and type [%d] saved!\n", fileName, shaderId, shaderType);
				return true;
			}
		}

		// no space free
		GameLogger::Log(MessageType::cFatal_Error, "Unable to save shader [%s] with id [%d] and type [%d]! No availiable space in array!\n", fileName, shaderId, shaderType);
		return false;
	}

	bool ShaderProgram::DeleteShaders()
	{
		// no errors yet!
		bool good = true;
		int numDeleted = 0;

		// loop through all shaders
		for (int i = 0; i < maxShaders; ++i)
		{
			// if used/not deleted
			if (!m_shaderDeleted[i])
			{
				// delete it
				glDeleteShader(m_shaders[i].id);

				// test for errors in deleting it
				if (MyGL::TestForError(MessageType::cError, "Error deleting shaders"))
				{
					GameLogger::Log(MessageType::cError, "Errors present when deleting shader [%d]!\n", m_shaders[i].id);
					good = false;
					continue; // don't mark shader as deleted if errors showed up when trying to delete it
				}

				// mark it as deleted
				m_shaderDeleted[i] = GL_TRUE;
				numDeleted++;
			}
		}

		GameLogger::Log(MessageType::Info, "Deleted [%d] shaders successfully!\n", numDeleted);
		return good;
	}

	bool ShaderProgram::DeleteProgram()
	{
		// if not already deleted, delete program
		if (!m_deleted)
		{
			// delete program
			glDeleteProgram(m_id);

			// check for errors trying to delete program
			if (MyGL::TestForError(MessageType::cError, "Errors deleting program"))
			{
				GameLogger::Log(MessageType::cError, "Errors detected when trying to delete program [%d]!\n", m_id);
				return false;
			}

			// mark program as deleted if no erros found
			m_deleted = GL_TRUE;
			GameLogger::Log(MessageType::Info, "Deleted program [%d]!\n", m_id);
		}

		return true;
	}

	// gets the number of attributes for the shader
	int ShaderProgram::GetNumAttributes()
	{
		// make and initialize variable to store number of attributes in the shader program
		int numAttributes = 0;

		// query opengl for the number of active attributes
		glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttributes);

		// check to make sure no errors occurred during the querying of the shader
		if (MyGL::TestForError(MessageType::cError, "Errors in getting num attributes for shader [%d]", m_id)) { return -1; }

		// return the number of attributes
		return numAttributes;
	}

	// gets the number of uniforms for the shader
	int ShaderProgram::GetNumUniforms()
	{
		// the number of uniforms
		int numUniforms = 0;

		// query opengl for the number of active uniforms
		glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

		// check to make sure no errors occurred during the querying of the shader
		if (MyGL::TestForError(MessageType::cError, "Errors in getting num uniforms for shader [%d]", m_id)) { return -1; }

		return numUniforms;
	}

	const int NUM_ATTRIB_VALUES = 2;
	const int BUFFER_SIZE = 25;
	// displays a single attribute for the shader
	bool ShaderProgram::DisplaySingleAttribute(int attribNum)
	{
		// variables to store results of gl calls
		int attribValues[NUM_ATTRIB_VALUES]{ 0 };
		GLenum attribValueIdentifiers[NUM_ATTRIB_VALUES]{ GL_LOCATION, GL_TYPE }; // grab the location, and type
		char attribName[BUFFER_SIZE]{ 0 }; // hold name string

		// gl calls to query for the information we desire
		glGetProgramResourceiv(m_id, GL_PROGRAM_INPUT, attribNum, NUM_ATTRIB_VALUES, &attribValueIdentifiers[0], NUM_ATTRIB_VALUES, nullptr, &attribValues[0]);
		glGetProgramResourceName(m_id, GL_PROGRAM_INPUT, attribNum, BUFFER_SIZE, nullptr, attribName);

		// check if errors happened, respond accordingly
		if (MyGL::TestForError(MessageType::cError, "Errors in Display Single Attribute")) { return false; }

		// output
		GameLogger::Log(MessageType::ConsoleOnly, "%10s%10d%30s%20s\n", "Attrib", attribValues[0], attribName, GetTypeString(attribValues[1]));
		return true;
	}

	const int NUM_UNIFORM_VALUES = 3;
	// displays information for a single uniform for the shader
	bool ShaderProgram::DisplaySingleUniform(int uniformNum, bool displayUniformBlocks)
	{
		// variables to store results of gl calls
		int uniformValues[NUM_UNIFORM_VALUES]{ 0 };
		GLenum uniformValueIdentifiers[NUM_UNIFORM_VALUES]{ GL_LOCATION, GL_TYPE, GL_BLOCK_INDEX }; // grab the location, and type
		char uniformName[BUFFER_SIZE]{ 0 }; // hold name string

		// gl calls to query for the information we desire
		glGetProgramResourceiv(m_id, GL_UNIFORM, uniformNum, NUM_UNIFORM_VALUES, &uniformValueIdentifiers[0], NUM_UNIFORM_VALUES, nullptr, &uniformValues[0]);
		glGetProgramResourceName(m_id, GL_UNIFORM, uniformNum, BUFFER_SIZE, nullptr, uniformName);

		// check if errors happened, respond accordingly
		if (MyGL::TestForError(MessageType::cError, "Errors in Display Single uniform")) { return false; }

		// output
		bool isUniformBlock = (uniformValues[2] != -1);
		if (!isUniformBlock || (displayUniformBlocks))
		{
			GameLogger::Log(MessageType::ConsoleOnly, "%10s%10d%30s%20s\n", "Uniform", uniformValues[0], uniformName, GetTypeString(uniformValues[1]));
		}

		// indicate success
		return true;
	}

	// TODO: CALL THIS METHOD
	bool ShaderProgram::DisplayShaderInfoHeader()
	{
		GameLogger::Log(MessageType::ConsoleOnly, "Shader Program ID [%d] [%s]\n", m_id, m_shaders[0].name); // TODO: INVESTIGATE [0]
		GameLogger::Log(MessageType::ConsoleOnly, "%10s%10s%30s%20s\n", "Item", "Loc", "Name", "Type");
		return true;
	}

	// converts the integer returned when querying for GL_TYPE to a string that is more readable
	char messages[]{"float\0vec2\0vec3\0vec4\0double\0int\0unsigned int\0bool\0mat2\0mat3\0mat4\0textureSampler\0Unknown Type"};
	int indices[]{ 0, 6, 11, 16, 20, 28, 31, 44, 49, 54, 60, 65, 80 };
	const char * ShaderProgram::GetTypeString(GLenum type)
	{
		switch (type)
		{
		case GL_FLOAT: return &messages[indices[0]];
		case GL_FLOAT_VEC2: return &messages[indices[1]];
		case GL_FLOAT_VEC3: return &messages[indices[2]];
		case GL_FLOAT_VEC4: return &messages[indices[3]];
		case GL_DOUBLE: return &messages[indices[4]];
		case GL_INT: return &messages[indices[5]];
		case GL_UNSIGNED_INT: return &messages[indices[6]];
		case GL_BOOL: return &messages[indices[7]];
		case GL_FLOAT_MAT2: return &messages[indices[8]];
		case GL_FLOAT_MAT3: return &messages[indices[9]];
		case GL_FLOAT_MAT4: return &messages[indices[10]];
		case GL_SAMPLER_2D: return &messages[indices[11]];
		default: return &messages[indices[12]];
		}
	}
}


