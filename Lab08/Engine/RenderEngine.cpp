#include "RenderEngine.h"
#include "Mesh.h"
#include "GraphicalObject.h"
#include "BufferManager.h"
#include "GameLogger.h"
#include "MyGL.h"

// Justin Furtado
// 7/14/2016
// RenderEngine.h
// Manages the rendering of graphical objects

namespace Engine
{
	ShaderProgram RenderEngine::s_shaderPrograms[MAX_SHADER_PROGRAMS];
	GLuint RenderEngine::s_nextShaderProgram = 0;

	bool RenderEngine::Initialize(ShaderProgram *pPrograms, GLint shaderProgramCount)
	{
		if (!CopyShaderPrograms(pPrograms, shaderProgramCount))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to initialize render engine! Could not copy shader programs!\n");
			return false;
		}

		if (!BufferManager::Initialize())
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not initialize RenderEngine! BufferManager failed to initialize!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "Render Engine Initialized successfully!\n");
		return true;
	}

	bool RenderEngine::Shutdown()
	{
		if (!BufferManager::Shutdown()) { return false; }

		GameLogger::Log(MessageType::Process, "Render Engine Shutdown successfully!\n");
		return true;
	}

	bool RenderEngine::AddMesh(Mesh * pMeshToAdd)
	{
		if (!BufferManager::AddMesh(pMeshToAdd))
		{
			GameLogger::Log(MessageType::cError, "RenderEngine could not add mesh! BufferManager failed to add mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "RenderEngine added an indexed mesh to BufferManager successfully!\n");
		return true;
	}

	bool RenderEngine::AddGraphicalObject(GraphicalObject * pGraphicalObjectToAdd)
	{
		if (!BufferManager::AddGraphicalObject(pGraphicalObjectToAdd))
		{
			GameLogger::Log(MessageType::cError, "RenderEngine could not add graphical object! BufferManager failed to add graphical object!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "RenderEngine added a graphical object to buffer manager!\n");
		return true;
	}

	void RenderEngine::RemoveGraphicalObject(GraphicalObject * pGraphicalObjectToRemove)
	{
		BufferManager::RemoveGraphicalObject(pGraphicalObjectToRemove);
	}

	bool RenderEngine::Draw()
	{
		// for each buffer group
		for (int i = 0; i < BufferManager::GetNextBufferGroup(); ++i)
		{
			// convenience pointer :)
			BufferGroup *pCurrentBufferGroup = BufferManager::GetBufferGroups() + i;

			// get shader program for current group
			ShaderProgram *pCurrentProgram = GetShaderProgramByID(pCurrentBufferGroup->GetShaderProgramID());

			// if invalid don't stop drawing everything, just don't draw things from this group
			if (!pCurrentProgram)
			{
				GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group [%d]! Shader program was not found in render engine!\n", i);
				continue;
			}

			// if not invalid, use it
			if (!pCurrentProgram->UseProgram())
			{
				GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group [%d]! Will not draw objects from that group!\n", i);
				continue; // move on to next buffer group if unable to use shader program
			}

			// grouped based on culling, set culling once for the whole group instead of for every object!
			if (pCurrentBufferGroup->BufferGroupDoesCull()) { 
				glEnable(GL_CULL_FACE); 
			}
			else {
				glDisable(GL_CULL_FACE); 
			}

			// for each buffer info in the specific buffer group
			for (int j = 0; j < pCurrentBufferGroup->GetNextBufferInfo(); ++j)
			{
				// convenience pointer
				BufferInfo *pCurrentBufferInfo = pCurrentBufferGroup->GetBufferInfos() + j;

				// validate input
				if (!pCurrentBufferInfo->GetHeadNode())
				{
					//GameLogger::Log(MessageType::cWarning, "Tried to draw buffer info [%d] from buffer group [%d] but linked list was empty!\n", j, i);
					continue; // TESTING
				}

				// attempt to set attribs and bind buffers
				if (!SetupDrawingEnvironment(pCurrentBufferInfo)) { GameLogger::Log(MessageType::cWarning, "Failed to setup drawing environment for buffer info [%d] of buffer group [%d]! Will not draw from list!\n", j, i); continue; }

				// Loop through and draw all graphical objects in list
				GraphicalObject *currentNode = pCurrentBufferInfo->GetHeadNode();

				while (currentNode)
				{
					if (currentNode->IsEnabled())
					{
						// call the callback
						currentNode->PassUniforms();
						currentNode->CallCallback();

						// draw
						if (currentNode->GetMeshPointer()->IsIndexed())
						{
							glDrawElementsBaseVertex(currentNode->GetMeshPointer()->GetMeshMode(), currentNode->GetMeshPointer()->GetIndexCount(),
								GetIndexType(currentNode->GetMeshPointer()->GetIndexSize()),
								(void *)(currentNode->GetMeshPointer()->GetRenderInfoPtr()->indexBufferOffset),
								currentNode->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / currentNode->GetMeshPointer()->GetSizeOfVertex());
						}
						else
						{
							glDrawArrays(currentNode->GetMeshPointer()->GetMeshMode(),
								currentNode->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / currentNode->GetMeshPointer()->GetSizeOfVertex(),
								currentNode->GetMeshPointer()->GetVertexCount());
						}
					}

					// move to next node
					currentNode = currentNode->GetNextDrawingNode();
				}

			}
		}

		return true;
	}

	bool RenderEngine::DrawSingleObjectDifferently(GraphicalObject * pGob, void * pPersp, void * pLook, void *pTexId, int lookLoc, int perspLoc, int texLoc)
	{
		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "BEFORE METHOD")) { return false; }

		// get pointers to the correct buffer info and group so we can setup the drawing enivornment properly for this object
		BufferGroup *pCurrentBufferGroup = nullptr;
		BufferInfo *pCurrentBufferInfo = nullptr;

		// search for the correct buffer info and group
		bool found = false;
		for (int i = 0; i < BufferManager::GetNextBufferGroup() && !found; ++i)
		{
			BufferGroup * pBG = BufferManager::GetBufferGroups() + i;
			for (int j = 0; j < pBG->GetNextBufferInfo() && !found; ++j)
			{
				BufferInfo *pBI = pBG->GetBufferInfos() + j;
				if (pBI->BelongsInBuffer(pGob)) { pCurrentBufferInfo = pBI; pCurrentBufferGroup = pBG; found = true; }
			}
		}
		
		// if it is not found, explode violently
		if (!found) { GameLogger::Log(MessageType::cError, "Failed to DrawObjectWithLookAtAndPerspective! Could not find buffer object belongs in!\n"); return false; }

		// convenience pointer :)
		// get shader program for current group
		ShaderProgram *pCurrentProgram = GetShaderProgramByID(pCurrentBufferGroup->GetShaderProgramID());

		// if invalid don't stop drawing everything, just don't draw things from this group
		if (!pCurrentProgram)
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Shader program was not found in render engine!\n");
			return false;
		}

		// if not invalid, use it
		if (!pCurrentProgram->UseProgram())
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Will not draw objects from that group!\n");
			return false;
		}

		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot BEFORE CULLING")) { return false; }

		// grouped based on culling, set culling once for the whole group instead of for every object!
		if (pCurrentBufferGroup->BufferGroupDoesCull()) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot IN CULLING")) { return false; }


		// attempt to set attribs and bind buffers
		if (!SetupDrawingEnvironment(pCurrentBufferInfo)) { GameLogger::Log(MessageType::cWarning, "Failed to setup drawing environment for buffer info in single draw! Will not draw from list!\n"); return false; }

		if (pGob->IsEnabled())
		{
			// get pointers to the uniform data pointers so we can do things to the pointers
			void **pLookPtr = pGob->GetUniformDataPtrPtrByLoc(lookLoc);
			void **pPerspPtr = pGob->GetUniformDataPtrPtrByLoc(perspLoc);
			void **pTexPtr = pGob->GetUniformDataPtrPtrByLoc(texLoc);

			// make a copy of the pointers pointed to by the pointers so we can restore their values
			void *pLookPrior = pLookPtr ? *pLookPtr : nullptr;
			void *pPerspPrior = pPerspPtr ? *pPerspPtr : nullptr;
			void *pTexPrior = pTexPtr ? *pTexPtr : nullptr;

			// set the pointers pointed to by the pointers to point to the new matrices
			if (pLookPtr) { *pLookPtr = pLook; }
			if (pPerspPtr) { *pPerspPtr = pPersp; }
			if (pTexPtr) { *pTexPtr = pTexId; }

			// call the callback, pass the uniforms
			// if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot BEFORE UNIFORMS")) { return false; }

			pGob->PassUniforms();
			pGob->CallCallback();
			// if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot IN UNIFORMS")) { return false; }


			// if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot BEFORE DRAWING")) { return false; }

			// draw
			if (pGob->GetMeshPointer()->IsIndexed())
			{
				glDrawElementsBaseVertex(pGob->GetMeshPointer()->GetMeshMode(), pGob->GetMeshPointer()->GetIndexCount(),
					GetIndexType(pGob->GetMeshPointer()->GetIndexSize()),
					(void *)(pGob->GetMeshPointer()->GetRenderInfoPtr()->indexBufferOffset),
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex());
			}
			else
			{
				glDrawArrays(pGob->GetMeshPointer()->GetMeshMode(),
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex(),
					pGob->GetMeshPointer()->GetVertexCount());
			}
			// if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot IN DRAWING")) { return false; }

			//	if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot IN METHOD")) { return false; }

			// restore the pointers pointed to by the pointers to what they were pointing to befor
			if (pLookPtr) { *pLookPtr = pLookPrior; }
			if (pPerspPtr) { *pPerspPtr = pPerspPrior; }
			if (pTexPtr) { *pTexPtr = pTexPrior; }
		}

		return true;
	}

	bool RenderEngine::DrawSingleObjectRegularly(GraphicalObject * pGob)
	{
		return DrawSingleObjectDifferently(pGob, nullptr, nullptr, nullptr, -1, -1, -1);
	}

	bool RenderEngine::DrawSingleObjectWithDifferentMeshMode(GraphicalObject * pGob, GLenum meshMode)
	{
		// get pointers to the correct buffer info and group so we can setup the drawing enivornment properly for this object
		BufferGroup *pCurrentBufferGroup = nullptr;
		BufferInfo *pCurrentBufferInfo = nullptr;

		// search for the correct buffer info and group
		bool found = false;
		for (int i = 0; i < BufferManager::GetNextBufferGroup() && !found; ++i)
		{
			BufferGroup * pBG = BufferManager::GetBufferGroups() + i;
			for (int j = 0; j < pBG->GetNextBufferInfo() && !found; ++j)
			{
				BufferInfo *pBI = pBG->GetBufferInfos() + j;
				if (pBI->BelongsInBuffer(pGob)) { pCurrentBufferInfo = pBI; pCurrentBufferGroup = pBG; found = true; }
			}
		}

		// if it is not found, explode violently
		if (!found) { GameLogger::Log(MessageType::cError, "Failed to DrawObjectWithLookAtAndPerspective! Could not find buffer object belongs in!\n"); return false; }

		// convenience pointer :)
		// get shader program for current group
		ShaderProgram *pCurrentProgram = GetShaderProgramByID(pCurrentBufferGroup->GetShaderProgramID());

		// if invalid don't stop drawing everything, just don't draw things from this group
		if (!pCurrentProgram)
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Shader program was not found in render engine!\n");
			return false;
		}

		// if not invalid, use it
		if (!pCurrentProgram->UseProgram())
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Will not draw objects from that group!\n");
			return false;
		}

		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot BEFORE CULLING")) { return false; }

		// grouped based on culling, set culling once for the whole group instead of for every object!
		if (pCurrentBufferGroup->BufferGroupDoesCull()) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot IN CULLING")) { return false; }


		// attempt to set attribs and bind buffers
		if (!SetupDrawingEnvironment(pCurrentBufferInfo)) { GameLogger::Log(MessageType::cWarning, "Failed to setup drawing environment for buffer info in single draw! Will not draw from list!\n"); return false; }

		if (pGob->IsEnabled())
		{
			// call the callback, pass the uniforms
			pGob->PassUniforms();
			pGob->CallCallback();

			// draw
			if (pGob->GetMeshPointer()->IsIndexed())
			{
				glDrawElementsBaseVertex(meshMode, pGob->GetMeshPointer()->GetIndexCount(),
					GetIndexType(pGob->GetMeshPointer()->GetIndexSize()),
					(void *)(pGob->GetMeshPointer()->GetRenderInfoPtr()->indexBufferOffset),
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex());
			}
			else
			{
				glDrawArrays(meshMode,
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex(),
					pGob->GetMeshPointer()->GetVertexCount());
			}
		}

		return true;
	}

	// TODO: REFACTOR THIS SO MUCH
	bool RenderEngine::DrawInstanced(GraphicalObject * pGob, InstanceBuffer *pInstanceBuffer)
	{
		// get pointers to the correct buffer info and group so we can setup the drawing enivornment properly for this object
		BufferGroup *pCurrentBufferGroup = nullptr;
		BufferInfo *pCurrentBufferInfo = nullptr;

		// search for the correct buffer info and group
		bool found = false;
		for (int i = 0; i < BufferManager::GetNextBufferGroup() && !found; ++i)
		{
			BufferGroup * pBG = BufferManager::GetBufferGroups() + i;
			for (int j = 0; j < pBG->GetNextBufferInfo() && !found; ++j)
			{
				BufferInfo *pBI = pBG->GetBufferInfos() + j;
				if (pBI->BelongsInBuffer(pGob)) { pCurrentBufferInfo = pBI; pCurrentBufferGroup = pBG; found = true; }
			}
		}

		// if it is not found, explode violently
		if (!found) { GameLogger::Log(MessageType::cError, "Failed to DrawInstanced! Could not find buffer object belongs in!\n"); return false; }

		// convenience pointer :)
		// get shader program for current group
		ShaderProgram *pCurrentProgram = GetShaderProgramByID(pCurrentBufferGroup->GetShaderProgramID());

		// if invalid don't stop drawing everything, just don't draw things from this group
		if (!pCurrentProgram)
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Shader program was not found in render engine!\n");
			return false;
		}

		// if not invalid, use it
		if (!pCurrentProgram->UseProgram())
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Will not draw objects from that group!\n");
			return false;
		}

		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot BEFORE CULLING")) { return false; }

		// grouped based on culling, set culling once for the whole group instead of for every object!
		if (pCurrentBufferGroup->BufferGroupDoesCull()) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot IN CULLING")) { return false; }


		// attempt to set attribs and bind buffers
		int outIndex = 0;
		if (!SetupDrawingEnvironment(pCurrentBufferInfo, &outIndex)) { GameLogger::Log(MessageType::cWarning, "Failed to setup drawing environment for buffer info in single draw! Will not draw from list!\n"); return false; }
		pInstanceBuffer->SetupAttrib(outIndex);

		if (pGob->IsEnabled())
		{
			// call the callback, pass the uniforms
			pGob->PassUniforms();
			pGob->CallCallback();

			// draw
			if (pGob->GetMeshPointer()->IsIndexed())
			{
				glDrawElementsInstancedBaseVertex(pGob->GetMeshPointer()->GetMeshMode(), pGob->GetMeshPointer()->GetIndexCount(),
					GetIndexType(pGob->GetMeshPointer()->GetIndexSize()),
					(void *)(pGob->GetMeshPointer()->GetRenderInfoPtr()->indexBufferOffset), pInstanceBuffer->GetCount(),
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex());
			}
			else
			{
				glDrawArraysInstanced(pGob->GetMeshPointer()->GetMeshMode(),
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex(),
					pGob->GetMeshPointer()->GetVertexCount(), pInstanceBuffer->GetCount());
			}
		}

		return true;
	}

	bool RenderEngine::DrawInstanced(GraphicalObject * pGob, int count)
	{
		// get pointers to the correct buffer info and group so we can setup the drawing enivornment properly for this object
		BufferGroup *pCurrentBufferGroup = nullptr;
		BufferInfo *pCurrentBufferInfo = nullptr;

		// search for the correct buffer info and group
		bool found = false;
		for (int i = 0; i < BufferManager::GetNextBufferGroup() && !found; ++i)
		{
			BufferGroup * pBG = BufferManager::GetBufferGroups() + i;
			for (int j = 0; j < pBG->GetNextBufferInfo() && !found; ++j)
			{
				BufferInfo *pBI = pBG->GetBufferInfos() + j;
				if (pBI->BelongsInBuffer(pGob)) { pCurrentBufferInfo = pBI; pCurrentBufferGroup = pBG; found = true; }
			}
		}

		// if it is not found, explode violently
		if (!found) { GameLogger::Log(MessageType::cError, "Failed to DrawInstanced! Could not find buffer object belongs in!\n"); return false; }

		// convenience pointer :)
		// get shader program for current group
		ShaderProgram *pCurrentProgram = GetShaderProgramByID(pCurrentBufferGroup->GetShaderProgramID());

		// if invalid don't stop drawing everything, just don't draw things from this group
		if (!pCurrentProgram)
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Shader program was not found in render engine!\n");
			return false;
		}

		// if not invalid, use it
		if (!pCurrentProgram->UseProgram())
		{
			GameLogger::Log(MessageType::cWarning, "Failed to use shader program for buffer group in single draw! Will not draw objects from that group!\n");
			return false;
		}

		//if (Engine::MyGL::TestForError(Engine::MessageType::ConsoleOnly, "Spot BEFORE CULLING")) { return false; }

		// grouped based on culling, set culling once for the whole group instead of for every object!
		if (pCurrentBufferGroup->BufferGroupDoesCull()) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		// attempt to set attribs and bind buffers
		if (!SetupDrawingEnvironment(pCurrentBufferInfo)) { GameLogger::Log(MessageType::cWarning, "Failed to setup drawing environment for buffer info in single draw! Will not draw from list!\n"); return false; }

		if (pGob->IsEnabled())
		{
			// call the callback, pass the uniforms
			pGob->PassUniforms();
			pGob->CallCallback();

			// draw
			if (pGob->GetMeshPointer()->IsIndexed())
			{
				glDrawElementsInstancedBaseVertex(pGob->GetMeshPointer()->GetMeshMode(), pGob->GetMeshPointer()->GetIndexCount(),
					GetIndexType(pGob->GetMeshPointer()->GetIndexSize()),
					(void *)(pGob->GetMeshPointer()->GetRenderInfoPtr()->indexBufferOffset), count,
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex());
			}
			else
			{
				glDrawArraysInstanced(pGob->GetMeshPointer()->GetMeshMode(),
					pGob->GetMeshPointer()->GetRenderInfoPtr()->vertexBufferOffset / pGob->GetMeshPointer()->GetSizeOfVertex(),
					pGob->GetMeshPointer()->GetVertexCount(), count);
			}
		}

		return true;
	}

	void RenderEngine::LogStats()
	{
		BufferManager::ConsoleLogStats();
	}

	bool RenderEngine::SetupDrawingEnvironment(BufferInfo *pBufferInfo, int *outIndex)
	{
		// use the correct buffers
		glBindBuffer(GL_ARRAY_BUFFER, pBufferInfo->GetVertexBufferID());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBufferInfo->GetIndexBufferID());

		// error checking
		if (MyGL::TestForError(MessageType::cWarning, "SetupDrawingEnvironment Errors"))
		{
			GameLogger::Log(MessageType::cWarning, "Failed to set up drawing environment for buffer info!\n");
			return false;
		}

		// success
		return SetupAttribs(pBufferInfo, outIndex);
	}

	const int POSITION_COUNT = 3;
	const int COLOR_COUNT = 3;
	const int TEXTURE_COUNT = 2;
	const int NORMAL_COUNT = 3;
	const GLenum TYPE = GL_FLOAT;
	const GLboolean NORMALIZED = GL_FALSE;
	bool RenderEngine::SetupAttribs(BufferInfo *pBufferInfo, int *outIndex)
	{
		int attribIndex = 0;
		int offset = 0;

		// dynamicaly setup attribs based on vertex format
		if (pBufferInfo->GetHeadNode()->GetMeshPointer()->GetVertexFormat() & VertexFormat::HasPosition) { glEnableVertexAttribArray(attribIndex);	glVertexAttribPointer(attribIndex++, POSITION_COUNT, TYPE, NORMALIZED, pBufferInfo->GetHeadNode()->GetMeshPointer()->GetSizeOfVertex(), (void *)offset); offset += POSITION_BYTES; }
		if (pBufferInfo->GetHeadNode()->GetMeshPointer()->GetVertexFormat() & VertexFormat::HasColor) { glEnableVertexAttribArray(attribIndex);	glVertexAttribPointer(attribIndex++, COLOR_COUNT, TYPE, NORMALIZED, pBufferInfo->GetHeadNode()->GetMeshPointer()->GetSizeOfVertex(), (void *)offset); offset += COLOR_BYTES; }
		if (pBufferInfo->GetHeadNode()->GetMeshPointer()->GetVertexFormat() & VertexFormat::HasTexture) { glEnableVertexAttribArray(attribIndex);	glVertexAttribPointer(attribIndex++, TEXTURE_COUNT, TYPE, NORMALIZED, pBufferInfo->GetHeadNode()->GetMeshPointer()->GetSizeOfVertex(), (void *)offset); offset += TEXTURE_BYTES; }
		if (pBufferInfo->GetHeadNode()->GetMeshPointer()->GetVertexFormat() & VertexFormat::HasNormal) { glEnableVertexAttribArray(attribIndex);	glVertexAttribPointer(attribIndex++, NORMAL_COUNT, TYPE, NORMALIZED, pBufferInfo->GetHeadNode()->GetMeshPointer()->GetSizeOfVertex(), (void *)offset); offset += NORMAL_BYTES; }
		if (outIndex) { *outIndex = attribIndex; }

		// error checking
		if (MyGL::TestForError(MessageType::cWarning, "SetupAttribs Errors"))
		{
			GameLogger::Log(MessageType::cWarning, "Failed to set up attribs for buffer info!\n");
			return false;
		}

		// success
		return true;
	}

	bool RenderEngine::CopyShaderPrograms(ShaderProgram * pShaderPrograms, GLint shaderProgramCount)
	{
		// error checking
		if (shaderProgramCount < 0) { GameLogger::Log(MessageType::cFatal_Error, "Failed to copy shader programs to render engine! Cannot copy less than 0 programs!\n"); return false; }
		if (shaderProgramCount > MAX_SHADER_PROGRAMS) { GameLogger::Log(MessageType::cFatal_Error, "Failed to copy shader programs to render engine! Cannot copy more than maximum of [%d] shader programs!\n", MAX_SHADER_PROGRAMS); return false; }
		if (!pShaderPrograms) { GameLogger::Log(MessageType::cFatal_Error, "Tried to copy shader programs but nullptr was passed as location to copy from!\n"); return false; }

		// do the copy
		for (int i = 0; i < shaderProgramCount; ++i)
		{
			s_shaderPrograms[i] = *(pShaderPrograms + i);
		}

		// update info
		s_nextShaderProgram = shaderProgramCount;

		// success
		GameLogger::Log(MessageType::Process, "Successfully copied [%d] shader programs to render engine!\n", shaderProgramCount);
		return true;
	}

	ShaderProgram * RenderEngine::GetShaderProgramByID(GLint shaderProgramID)
	{
		// loop through programs
		for (unsigned int i = 0; i < s_nextShaderProgram; ++i)
		{
			// compare proram ids
			if (s_shaderPrograms[i].GetProgramId() == shaderProgramID)
			{
				// if match, return it
				return &s_shaderPrograms[i];
			}
		}

		// indicate no program was found
		return nullptr;
	}

	GLenum RenderEngine::GetIndexType(IndexSizeInBytes indexSize)
	{
		switch (indexSize)
		{
		case IndexSizeInBytes::Ubyte: return GL_UNSIGNED_BYTE;
		case IndexSizeInBytes::Ushort: return GL_UNSIGNED_SHORT;
		case IndexSizeInBytes::Uint:   return GL_UNSIGNED_INT;
		default:
			GameLogger::Log(MessageType::cProgrammer_Error, "Unknown index size (%d) in GetIndexType ", indexSize);
			return GL_FALSE;
		}
	}
}
