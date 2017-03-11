#include "ShapeGenerator.h"

#include "BitmapLoader.h"
#include "GameLogger.h"
#include "Mesh.h"
#include "ColorVertex.h"
#include "GraphicalObject.h"
#include "RenderEngine.h"
#include "StringFuncs.h"
#include "MathUtility.h"

// Justin Furtado
// 6/28/2016
// ShapeGenerator.cpp
// Generates shapes

namespace Engine
{
	Mesh ShapeGenerator::frustumMesh;
	Mesh ShapeGenerator::cubeMesh;
	Mesh ShapeGenerator::lightingCubeMesh;
	Mesh ShapeGenerator::normalCubeMesh;
	Mesh ShapeGenerator::houseMesh;
	Mesh ShapeGenerator::tetrahedronMesh;
	Mesh ShapeGenerator::gridMesh;
	Mesh ShapeGenerator::debugCubeMesh;
	Mesh ShapeGenerator::sphereMesh;
	Mesh ShapeGenerator::debugArrowMesh;
	Mesh ShapeGenerator::horizontalPlaneMesh;
	Mesh ShapeGenerator::nearPlaneMeshNDC;
	const char *ShapeGenerator::s_sceneFileNames[MAX_SCENE_FILES]{ nullptr };
	Mesh *ShapeGenerator::s_sceneMeshes[MAX_SCENE_FILES]{ nullptr };
	Mesh *ShapeGenerator::s_pPointMeshes[MAX_POINT_MESHES]{ nullptr };
	Mesh *ShapeGenerator::s_demoQuadMeshes[MAX_QUAD_MESHES]{ nullptr };
	Mesh *ShapeGenerator::s_pTessalatedPlanes[MAX_TESSALATED_PLANES]{ nullptr };
	Vec3 ShapeGenerator::s_numTesselations[MAX_TESSALATED_PLANES];
	int ShapeGenerator::s_nextSceneFile = 0;
	int ShapeGenerator::s_nextPointMesh = 0;
	int ShapeGenerator::s_nextTessalatedPlane = 0;
	int ShapeGenerator::s_nextQuadMesh = 0;
	unsigned int ShapeGenerator::s_PCShaderID = 0;
	unsigned int ShapeGenerator::s_PShaderID = 0;
	unsigned int ShapeGenerator::s_PNShaderID = 0;

	ColorVertex ShapeGenerator::cubeColorVerts[CUBE_VERTEX_COUNT] = {
		/* First Face */
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top left
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left

														   /* Second Face */
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top left
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left

														   /* Third Face */
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom right
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top left
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left

														   /* Fourth Face */
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom right
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left

														   /* Fifth Face */
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top left
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left

														   /* Sixth Face */
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom right
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f),  // top left
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f) // bottom left
	};

	bool ShapeGenerator::MakeCube(GraphicalObject *pObject)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make cube! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupCubeMesh()) { return false; } first = false; }

		pObject->SetMeshPointer(&cubeMesh);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a cube!\n");
		return true;
	}

	Vertex ShapeGenerator::lightingCubeVerts[LIGHTING_CUBE_VERTEX_COUNT] = {
		/* First Face */
		Vec3(-1.0f, -1.0f, +1.0f), // bottom left
		Vec3(+1.0f, -1.0f, +1.0f), // bottom right
		Vec3(+1.0f, +1.0f, +1.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), // top right
		Vec3(-1.0f, +1.0f, +1.0f), // top left
		Vec3(-1.0f, -1.0f, +1.0f), // bottom left

								   /* Second Face */
		Vec3(+1.0f, -1.0f, +1.0f), // bottom left
		Vec3(+1.0f, -1.0f, -1.0f), // bottom right
		Vec3(+1.0f, +1.0f, -1.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), // top left
		Vec3(+1.0f, -1.0f, +1.0f), // bottom left

								   /* Third Face */
		Vec3(+1.0f, -1.0f, -1.0f), // bottom left
		Vec3(-1.0f, -1.0f, -1.0f), // bottom right
		Vec3(-1.0f, +1.0f, -1.0f), // top right
		Vec3(-1.0f, +1.0f, -1.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), // top left
		Vec3(+1.0f, -1.0f, -1.0f), // bottom left

								   /* Fourth Face */
		Vec3(-1.0f, -1.0f, -1.0f), // bottom left
		Vec3(-1.0f, -1.0f, +1.0f), // bottom right
		Vec3(-1.0f, +1.0f, +1.0f), // top right
		Vec3(-1.0f, +1.0f, +1.0f), // top right
		Vec3(-1.0f, +1.0f, -1.0f), // top left
		Vec3(-1.0f, -1.0f, -1.0f), // bottom left

								   /* Fifth Face */
		Vec3(-1.0f, +1.0f, -1.0f), // bottom left
		Vec3(-1.0f, +1.0f, +1.0f), // bottom right
		Vec3(+1.0f, +1.0f, +1.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), // top left
		Vec3(-1.0f, +1.0f, -1.0f), // bottom left

								   /* Sixth Face */
		Vec3(-1.0f, -1.0f, +1.0f), // bottom left
		Vec3(-1.0f, -1.0f, -1.0f), // bottom right
		Vec3(+1.0f, -1.0f, -1.0f), // top right
		Vec3(+1.0f, -1.0f, -1.0f), // top right
		Vec3(+1.0f, -1.0f, +1.0f),  // top left
		Vec3(-1.0f, -1.0f, +1.0f),// bottom left
	};

	bool ShapeGenerator::MakeLightingCube(GraphicalObject * pObject)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make lighting cube! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupLightingCubeMesh()) { return false; } first = false; }

		pObject->SetMeshPointer(&lightingCubeMesh);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a lighting cube!\n");
		return true;
	}


	ColorVertex ShapeGenerator::cubeNormalVerts[NORMAL_CUBE_VERTEX_COUNT] = {
		/* First Face */
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, +1.0f), // bottom left
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, +1.0f), // bottom right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, +1.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, +1.0f), // top right
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(0.0f, 0.0f, +1.0f), // top left
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, +1.0f), // bottom left

															/* Second Face */
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(+1.0f, 0.0f, 0.0f), // bottom left
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(+1.0f, 0.0f, 0.0f), // bottom right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(+1.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(+1.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(+1.0f, 0.0f, 0.0f), // top left
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(+1.0f, 0.0f, 0.0f), // bottom left

															/* Third Face */
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), // bottom left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), // bottom right
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), // top right
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), // top left
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), // bottom left

															/* Fourth Face */
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1.0f, 0.0f, 0.0f), // bottom left
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(-1.0f, 0.0f, 0.0f), // bottom right
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(-1.0f, 0.0f, 0.0f), // top right
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(-1.0f, 0.0f, 0.0f), // top right
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(-1.0f, 0.0f, 0.0f), // top left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1.0f, 0.0f, 0.0f), // bottom left

															/* Fifth Face */
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, +1.0f, 0.0f), // bottom left
		Vec3(-1.0f, +1.0f, +1.0f), Vec3(0.0f, +1.0f, 0.0f), // bottom right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, +1.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, +1.0f), Vec3(0.0f, +1.0f, 0.0f), // top right
		Vec3(+1.0f, +1.0f, -1.0f), Vec3(0.0f, +1.0f, 0.0f), // top left
		Vec3(-1.0f, +1.0f, -1.0f), Vec3(0.0f, +1.0f, 0.0f), // bottom left

															/* Sixth Face */
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, -1.0f, 0.0f), // bottom left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), // bottom right
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), // top right
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), // top right
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, -1.0f, 0.0f), // top left
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, -1.0f, 0.0f) // bottom left
	};

	bool ShapeGenerator::MakeFrustum(GraphicalObject * pObject, float near, float far, float aspect, float fovy)
	{
		// TODO: SUPPORT MULTIPLE FRUSTUMS OF DIFFERENT SIZES
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make a frustum! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupFrustum(near, far, aspect, fovy)) { return false; } first = false; }

		pObject->SetMeshPointer(&frustumMesh);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a normal cube!\n");
		return true;
	}

	bool ShapeGenerator::MakeNormalCube(GraphicalObject * pObject)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make a normal cube! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupNormalCube()) { return false; } first = false; }

		pObject->SetMeshPointer(&normalCubeMesh);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a normal cube!\n");
		return true;
	}

	ColorVertex ShapeGenerator::planeNormalVerts[PLANE_VERTEX_COUNT];

	bool ShapeGenerator::MakeHorizontalPlane(GraphicalObject * pObject, Vec3 upperLeft, Vec3 lowerRight, Vec3 color)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make a plane! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupHorizontalPlane(upperLeft, lowerRight)) { return false; } first = false; }

		pObject->SetMeshPointer(&horizontalPlaneMesh);
		pObject->GetMatPtr()->m_materialColor = color;

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a lighting cube!\n");
		return true;
	}

	bool ShapeGenerator::MakeTessalatedPlane(GraphicalObject * pObject, Vec3 upperLeft, Vec3 lowerRight, Vec3 numTessalations, int shaderId)
	{
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make a plane! Invalid graphical object pointer passed!\n"); return false; }
		
		// start counting planes, if one of same size is found, stop counting. This leaves the index equal to a matching plane if one is found, or at the next availiable index otherwise
		int planeIndex = 0;
		for (planeIndex = 0; planeIndex < s_nextTessalatedPlane; ++planeIndex) 
		{	
			if (fabsf(s_numTesselations[planeIndex].GetX() - numTessalations.GetX()) < 1.0f && fabsf(s_numTesselations[planeIndex].GetZ() - numTessalations.GetZ()) < 1.0f && (unsigned)shaderId == s_pTessalatedPlanes[planeIndex]->GetShaderProgramID()) { break; }
		}

		if (planeIndex >= MAX_TESSALATED_PLANES) { GameLogger::Log(MessageType::cError, "No room for new planes!\n"); return false; }


		// only setup mesh the first time a shape of this tessalation is being created, no wasted buffer space
		if (planeIndex == s_nextTessalatedPlane)
		{ 
			if (!SetupTessalatedPlane(upperLeft, lowerRight, numTessalations, shaderId)) { return false; } 
		}

		// point to the correct mesh
		pObject->SetMeshPointer(s_pTessalatedPlanes[planeIndex]);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a lighting cube!\n");
		return true;
	}

	ColorVertex ShapeGenerator::tetrahedronColorVerts[TETRAHEDRON_VERTEX_COUNT] = {
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Front left
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Front right
		Vec3(+0.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Back middle
		Vec3(+0.0f, +1.0f, +0.0f), Vec3(0.0f, 0.0f, 0.0f)  // Top middle
	};

	GLuint ShapeGenerator::tetrahedronIndices[TETRAHEDRON_INDEX_COUNT] = {
		0, 1, 3, // first face
		1, 2, 3, // second face
		2, 0, 3, // third face
		0, 2, 1  // fourth face
	};

	bool ShapeGenerator::MakeTetrahedron(GraphicalObject * pObject)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator could not make Tetrahedron! Invalid graphical object poitner passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupTetrahedronMesh()) { return false; } first = false; }

		pObject->SetMeshPointer(&tetrahedronMesh);

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully made a Tetrahedron!\n");
		return true;
	}

	ColorVertex ShapeGenerator::houseColorVerts[HOUSE_VERTEX_COUNT] = {
		/* Faces 1 & 2*/
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Left
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Right
		Vec3(+1.0f, +0.25f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Right
		Vec3(+0.0f, +1.0f, +0.0f),  Vec3(0.0f, 0.0f, 0.0f), // Top Middle
		Vec3(-1.0f, +0.25f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Left

															/* Faces 3 & 4*/
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Left
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Right
		Vec3(+1.0f, +0.25f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Right
		Vec3(+0.0f, +1.0f, +0.0f),  Vec3(0.0f, 0.0f, 0.0f), // Top Middle
		Vec3(+1.0f, +0.25f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Left

															/* Faces 5 & 6*/
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Right
		Vec3(-1.0f, +0.25f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Right
		Vec3(+0.0f, +1.0f, +0.0f),  Vec3(0.0f, 0.0f, 0.0f), // Top Middle
		Vec3(+1.0f, +0.25f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Left

															/* Faces 7 & 8*/
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Left
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Bottom Right
		Vec3(-1.0f, +0.25f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Right
		Vec3(+0.0f, +1.0f, +0.0f),  Vec3(0.0f, 0.0f, 0.0f), // Top Middle
		Vec3(-1.0f, +0.25f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // Middle Left

															/* Face 9*/
		Vec3(-1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom left
		Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // bottom right
		Vec3(+1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), // top right
		Vec3(+1.0f, -1.0f, +1.0f), Vec3(0.0f, 0.0f, 0.0f) // top left
	};

	GLuint ShapeGenerator::houseIndices[HOUSE_INDEX_COUNT] = {
		0, 1, 2, 2, 4, 0, 2, 3, 4, // first quad + tri
		5, 6, 7, 7, 9, 5, 7, 8, 9, // first quad + tri
		10, 11, 12, 12, 14, 10, 12, 13, 14, // first quad + tri
		15, 16, 17, 17, 19, 15, 17, 18, 19, // first quad + tri
		20, 21, 22, 22, 23, 20 // bottom
	};

	bool ShapeGenerator::MakeHouse(GraphicalObject * pObject)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator could not make a house! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space 
		if (first) { if (!SetupHouseMesh()) { return false; } first = false; }

		pObject->SetMeshPointer(&houseMesh);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a house!\n");
		return true;
	}

	Vertex ShapeGenerator::gridVerts[MAX_GRID_VERTS]{ Vec3(0.0f, 0.0f, 0.0f) };

	bool ShapeGenerator::MakeGrid(GraphicalObject *pObject, unsigned int width, unsigned int height, Vec3 color)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator could not make a grid! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupGridMesh(width, height, color)) { return false; } first = false; }

		pObject->SetMeshPointer(&gridMesh);
		pObject->GetMatPtr()->m_materialColor = color;

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a grid!\n");
		return true;
	}

	Vertex ShapeGenerator::debugCubeVerts[DEBUG_CUBE_VERTEX_COUNT] = {
		Vec3(-1.0f, +1.0f, +1.0f),
		Vec3(-1.0f, -1.0f, +1.0f),
		Vec3(+1.0f, -1.0f, +1.0f),
		Vec3(+1.0f, +1.0f, +1.0f),
		Vec3(+1.0f, +1.0f, -1.0f),
		Vec3(+1.0f, -1.0f, -1.0f),
		Vec3(-1.0f, -1.0f, -1.0f),
		Vec3(-1.0f, +1.0f, -1.0f)
	};

	GLuint ShapeGenerator::debugCubeIndices[DEBUG_CUBE_INDEX_COUNT] = {
		0, 1, 1, 2, 2, 3, 3, 4,
		4, 5, 5, 6, 6, 7, 7, 0,
		3, 0, 4, 7, 6, 1, 5, 2
	};

	bool ShapeGenerator::MakeDebugCube(GraphicalObject * pObject, Vec3 color)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator could not make a debug cube! Invalid graphical object pointer passed!\n"); return false; }

		if (first) { if (!SetupDebugCube(color)) { return false; } first = false; }

		pObject->SetMeshPointer(&debugCubeMesh);
		pObject->GetMatPtr()->m_materialColor = color;

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a debug cube!\n");
		return true;
	}

	const float h = 0.30f;
	const float w = 0.25f;
	const float d = 0.33f;
	ColorVertex ShapeGenerator::debugArrowColorVerts[DEBUG_ARROW_VERTEX_COUNT] = {
		// top face
		Vec3(+1.0f, +0.0f, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, +1.0f, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, -h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -1.0f, +d), Vec3(+0.0f, +0.0f, +0.0f),

		// bottom face
		Vec3(+1.0f, +0.0f, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -1.0f, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, -h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, +1.0f, -d), Vec3(+0.0f, +0.0f, +0.0f),

		// sides
		Vec3(+1.0f, +0.0f, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, +1.0f, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, -h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -h, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -1.0f, +d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(+1.0f, +0.0f, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -1.0f, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, -h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, -h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(-1.0f, h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, h, -d), Vec3(+0.0f, +0.0f, +0.0f),
		Vec3(w, +1.0f, -d), Vec3(+0.0f, +0.0f, +0.0f)
	};

	GLuint ShapeGenerator::debugArrowIndices[DEBUG_ARROW_INDEX_COUNT] = {
		0, 1, 6, 2, 4, 5, 2, 3, 4, 7, 8, 13, 9, 11, 12, 9, 10, 11, 21, 14,
		22, 22, 14, 20, 20, 23, 22, 19, 23, 20, 19, 24, 23, 24, 19, 18, 18, 25, 24, 18,
		17, 25, 25, 17, 26, 26, 17, 16, 16, 27, 26, 16, 15, 27, 27, 15, 14, 27, 14, 21
	};

	bool ShapeGenerator::MakeDebugArrow(GraphicalObject * pObject, Vec3 topColor, Vec3 sideColor)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator could not make a debug arrow! Invalid graphical object pointer passed!\n"); return false; }

		if (first) { if (!SetupDebugArrow(topColor, sideColor)) { return false; } first = false; }

		pObject->SetMeshPointer(&debugArrowMesh);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a debug arrow!\n");
		return true;
	}

	bool ShapeGenerator::MakeSphere(GraphicalObject * pObject, Vec3 color)
	{
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator could not make a sphere! Invalid graphical object pointer passed!\n"); return false; }

		if (first) { if (!SetupSphereMesh(color)) { return false; } first = false; }

		pObject->SetMeshPointer(&sphereMesh);
		pObject->GetMatPtr()->m_materialColor = color;

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a sphere!\n");
		return true;
	}


	bool ShapeGenerator::ReadSceneFile(const char * fileName, GraphicalObject * pObject, GLuint shaderProgramID, const char *texturePath, bool cullObject)
	{
		Mesh *pSceneMesh = FindMeshBySceneString(fileName, cullObject, shaderProgramID);

		// if the mesh doesn't exist set it up
		if (!pSceneMesh || (shaderProgramID != pSceneMesh->GetShaderProgramID()) || (cullObject != pSceneMesh->IsCullingEnabledForObject()))
		{
			if (!SetupSceneFile(fileName, shaderProgramID, texturePath, cullObject))
			{
				GameLogger::Log(MessageType::cError, "ShapeGenerator failed to ReadSceneFile()! failed to SetupSceneFile([%s])!\n", fileName);
				return false;
			}
		}

		// set scene mesh again... since it has been added
		pSceneMesh = FindMeshBySceneString(fileName, cullObject, shaderProgramID);

		// point to the scene mesh
		pObject->SetMeshPointer(pSceneMesh);

		// log message and indicate success
		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully read scene file [%s] into mesh!\n", fileName);
		return true;
	}

	// creates numPoints points randomly placed within the bounding volume specified by upperLeftBound and lowerRightBound
	bool ShapeGenerator::CreatePoints(GraphicalObject *pObject, int numPoints, Vec3 upperLeftBound, Vec3 lowerRightBound, int shaderId, bool colorOn, Vec3 color)
	{
		// validate inputs and prerequisites for creating mesh
		if (numPoints <= 0) { GameLogger::Log(MessageType::cWarning, "Tried to generate zero or less points!\n"); return false; }
		if (s_nextPointMesh >= MAX_POINT_MESHES) { GameLogger::Log(MessageType::cError, "Could not Create [%d] points between (%.3f, %.3f, %.3f) and (%.3f, %.35, %.3f)! Maximum number of point meshes [%d] would be exceeded!\n", numPoints, upperLeftBound.GetX(), upperLeftBound.GetY(), upperLeftBound.GetZ(), lowerRightBound.GetX(), lowerRightBound.GetY(), lowerRightBound.GetZ(), MAX_POINT_MESHES); return false; }

		// create the mesh
		int stride = colorOn ? 2 : 1;
		int numVecs = numPoints * stride;// numVerts = numPoints for position only, double for position color
		Vec3 *pVerts = new Vec3[numVecs];
		for (int i = 0; i < numVecs; i += stride)
		{
			pVerts[i] = MathUtility::Rand(upperLeftBound, lowerRightBound);
			if (colorOn) { pVerts[i+1] = color; }
		}
		
		// make the mesh
		s_pPointMeshes[s_nextPointMesh] = new Mesh(numPoints, 0, pVerts, nullptr, GL_POINTS, IndexSizeInBytes::Uint, shaderId, colorOn ? VertexFormat::PositionColor : VertexFormat::PositionOnly);

		// add the mesh to the render engine
		if (!Engine::RenderEngine::AddMesh(s_pPointMeshes[s_nextPointMesh])) { Engine::GameLogger::Log(MessageType::cError, "Failed to add points mesh to render engine!\n"); return false; }

		// if it succeeded, point to it, increment counter and indicate success
		pObject->SetMeshPointer(s_pPointMeshes[s_nextPointMesh]);
		s_nextPointMesh++;
		GameLogger::Log(MessageType::Process, "CreatePoints made [%d] points between (%.3f, %.3f, %.3f) and (%.3f, %.3f, %.3f)!\n", numPoints, upperLeftBound.GetX(), upperLeftBound.GetY(), upperLeftBound.GetZ(), lowerRightBound.GetX(), lowerRightBound.GetY(), lowerRightBound.GetZ());
		return true; 
	}

	// overload for sphere
	bool ShapeGenerator::CreatePoints(GraphicalObject * pObject, int numPoints, float thetaMin, float thetaMax, float phiMin, float phiMax, float velMin, float velMax, float spawnRate, int shaderId)
	{
		// validate inputs and prerequisites for creating mesh
		if (numPoints <= 0) { GameLogger::Log(MessageType::cWarning, "Tried to generate zero or less points!\n"); return false; }
		if (s_nextPointMesh >= MAX_POINT_MESHES) { GameLogger::Log(MessageType::cError, "Could not Create [%d] points in sphere! Maximum number of point meshes [%d] would be exceeded!\n", numPoints, MAX_POINT_MESHES); return false; }

		// create the mesh
		int stride = 5;
		int numFloats = numPoints * stride;// numVerts = numPoints for position only, double for position color
		float *pVerts = new float[numFloats];
		float time = 0.0f;
		for (int i = 0; i < numFloats; i += stride)
		{
			float phi = MathUtility::Rand(phiMin, phiMax);
			float theta = MathUtility::Rand(thetaMin, thetaMax);
			float vel = MathUtility::Rand(velMin, velMax);

			pVerts[i + 0] = vel*sinf(theta)*cosf(phi); // x
			pVerts[i + 1] = vel*cosf(theta); // y
			pVerts[i + 2] = vel*sinf(theta)*sinf(phi); // z
			pVerts[i + 3] = time; // spawn time
			pVerts[i + 4] = time; // NOT USED
			time += spawnRate;
		}

		// make the mesh
		s_pPointMeshes[s_nextPointMesh] = new Mesh(numPoints, 0, pVerts, nullptr, GL_POINTS, IndexSizeInBytes::Uint, shaderId, VertexFormat::PositionTexture, false);

		// add the mesh to the render engine
		if (!Engine::RenderEngine::AddMesh(s_pPointMeshes[s_nextPointMesh])) { Engine::GameLogger::Log(MessageType::cError, "Failed to add points mesh to render engine!\n"); return false; }

		// if it succeeded, point to it, increment counter and indicate success
		pObject->SetMeshPointer(s_pPointMeshes[s_nextPointMesh]);
		s_nextPointMesh++;
		GameLogger::Log(MessageType::Process, "CreatePoints made [%d] in a sphere!\n", numPoints);//, upperLeftBound.GetX(), upperLeftBound.GetY(), upperLeftBound.GetZ(), lowerRightBound.GetX(), lowerRightBound.GetY(), lowerRightBound.GetZ());
		return true;
	}

	bool ShapeGenerator::MakeNearPlanePlane(GraphicalObject * pObject, GLuint shaderProgramId)
	{
		// TODO: SUPPORT MULTIPLE FRUSTUMS OF DIFFERENT SIZES!?!??! maybe not needed  - just pasted comment
		static bool first = true;
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make a NearPlanePlane! Invalid graphical object pointer passed!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (first) { if (!SetupNearPlanePlane(shaderProgramId)) { return false; } first = false; }

		pObject->SetMeshPointer(&nearPlaneMeshNDC);

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a NearPlanePlane!\n");
		return true;
	}

	bool ShapeGenerator::MakeDemoQuad(GraphicalObject * pObject, GLuint shaderProgramID)
	{
		// TODO: SUPPORT MULTIPLE FRUSTUMS OF DIFFERENT SIZES!?!??! maybe not needed  - just pasted comment
		if (!pObject) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to make a DemoQuad! Invalid graphical object pointer passed!\n"); return false; }
		if (s_nextQuadMesh >= MAX_QUAD_MESHES) { Engine::GameLogger::Log(MessageType::cError, "Failed to make demo quad! Maximum exceeded!\n"); return false; }

		// only setup mesh the first time a shape of this type is being created, no wasted buffer space
		if (!SetupDemoQuad(shaderProgramID)) { return false; }
		pObject->SetMeshPointer(s_demoQuadMeshes[s_nextQuadMesh - 1]); // one added

		GameLogger::Log(MessageType::Process, "ShapeGenerator made a DemoQuad!\n");
		return true;
	}

	bool ShapeGenerator::Initialize(unsigned int pcShaderID, unsigned int pShaderID, unsigned int pnShaderID)
	{
		// set shaders to use
		s_PCShaderID = pcShaderID;
		s_PShaderID = pShaderID;
		s_PNShaderID = pnShaderID;

		if (!BitmapLoader::Initialize())
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to initialize ShapeGenerator! Could not initialize BitmapLoader!\n");
			return false;
		}

		// indicate success
		GameLogger::Log(MessageType::Process, "ShapeGenerator initialize was successful!\n");
		return true;
	}

	bool ShapeGenerator::Shutdown()
	{
		// clean up after scene verts and indices
		for (int i = 0; i < s_nextSceneFile; ++i)
		{
			delete s_sceneMeshes[i];
		}

		// clean up after point meshes
		for (int i = 0; i < s_nextPointMesh; ++i)
		{
			//delete[] s_pPointMeshes[i]->GetVertexPointer();
			delete s_pPointMeshes[i];
		}

		// clean up after tessalated planes
		for (int i = 0; i < s_nextTessalatedPlane; ++i)
		{
			delete s_pTessalatedPlanes[i];
		}

		// clean up after demo quads planes
		for (int i = 0; i < s_nextQuadMesh; ++i)
		{
			delete s_demoQuadMeshes[i];
		}

		if (!BitmapLoader::Shutdown())
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to shut down ShapeGenerator! Could not shut down BitmapLoader!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator shutdown was successful!\n");
		return true;
	}

	// R, G, B
	const int NUM_COLOR_VALUES = 3;
	void ShapeGenerator::SetColors(float *color0, int numVerts, int stride)
	{
		if (!color0) { GameLogger::Log(MessageType::cWarning, "Invalid pointer passed to set colors! No colors will be set!\n"); return; }
		if (stride <= 0) { stride = NUM_COLOR_VALUES; }
		for (int offset = 0; offset < (stride * numVerts); offset += stride)
		{
			for (int i = 0; i < NUM_COLOR_VALUES; ++i)
			{
				*(color0 + offset + i) = MathUtility::Rand(0.01f, 1.0f);
			}
		}
	}

	void ShapeGenerator::SetColors(float *color0, int numVerts, int stride, Vec3 color)
	{
		if (!color0) { GameLogger::Log(MessageType::cWarning, "Invalid pointer passed to set colors! No colors will be set!\n"); return; }
		if (stride <= 0) { stride = NUM_COLOR_VALUES; }
		for (int offset = 0; offset < (stride * numVerts); offset += stride)
		{
			*(color0 + offset) = color.GetR();
			*(color0 + offset + 1) = color.GetG();
			*(color0 + offset + 2) = color.GetB();
		}
	}

	void ShapeGenerator::CreateGridMesh(unsigned int widthInLines, unsigned int heightInLines)
	{
		for (unsigned int i = 0; i <= widthInLines * 2; i += 2)
		{
			gridVerts[i] = Vertex{ Vec3(i / 2.0f - (widthInLines / 2.0f), 0.0f, heightInLines / 2.0f) };
			gridVerts[i + 1] = Vertex{ Vec3(i / 2.0f - (widthInLines / 2.0f), 0.0f, -(heightInLines / 2.0f)) };
		}

		for (unsigned int i = 0; i <= heightInLines * 2; i += 2)
		{
			gridVerts[i + (widthInLines * 2) + 2] = Vertex{ Vec3((widthInLines / 2.0f), 0.0f, i / 2.0f - (heightInLines / 2.0f)) };
			gridVerts[i + (widthInLines * 2) + 3] = Vertex{ Vec3(-(widthInLines / 2.0f), 0.0f, i / 2.0f - (heightInLines / 2.0f)) };
		}
	}


	bool ShapeGenerator::SetupCubeMesh()
	{
		SetColors(cubeColorVerts[0].m_color.GetAddress(), CUBE_VERTEX_COUNT, ColorVertex::GetStride() / sizeof(GLfloat));
		cubeMesh = Mesh(CUBE_VERTEX_COUNT, 0, &cubeColorVerts[0], nullptr, GL_TRIANGLES, IndexSizeInBytes::Uint, s_PCShaderID, VertexFormat::PositionColor);

		if (!RenderEngine::AddMesh(&cubeMesh))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not initialize ShapeGenerator! Failed to add cube mesh to render engine!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator Successfully setup cube mesh!\n");
		return true;
	}

	bool ShapeGenerator::SetupLightingCubeMesh()
	{
		lightingCubeMesh = Mesh(LIGHTING_CUBE_VERTEX_COUNT, 0, &lightingCubeVerts[0], nullptr, GL_TRIANGLES, IndexSizeInBytes::Uint, s_PShaderID, VertexFormat::PositionOnly);

		if (!RenderEngine::AddMesh(&lightingCubeMesh))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Could not initialize ShapeGenerator! Failed to add lighting cube mesh to render engine!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator Successfully setup lighting cube mesh!\n");
		return true;
	}

	bool ShapeGenerator::SetupNormalCube()
	{
		normalCubeMesh = Mesh(NORMAL_CUBE_VERTEX_COUNT, 0, &cubeNormalVerts[0], nullptr, GL_TRIANGLES, IndexSizeInBytes::Uint, s_PNShaderID, VertexFormat::PositionNormal);

		if (!RenderEngine::AddMesh(&normalCubeMesh))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to initialize ShapeGenerator! Could not add normal cube mesh to render engine!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully setup normal cube mesh!\n");
		return true;
	}

	bool ShapeGenerator::SetupTetrahedronMesh()
	{
		SetColors(tetrahedronColorVerts[0].m_color.GetAddress(), TETRAHEDRON_VERTEX_COUNT, ColorVertex::GetStride() / sizeof(GLfloat));
		tetrahedronMesh = Mesh(TETRAHEDRON_VERTEX_COUNT, TETRAHEDRON_INDEX_COUNT, &tetrahedronColorVerts[0], &tetrahedronIndices[0], GL_TRIANGLES, IndexSizeInBytes::Uint, s_PCShaderID, VertexFormat::PositionColor);

		if (!RenderEngine::AddMesh(&tetrahedronMesh))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to initialize ShapeGenerator! Could not add tetrahedron mesh to render engine!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully setup tetrahedron mesh!\n");
		return true;
	}

	bool ShapeGenerator::SetupHouseMesh()
	{
		SetColors(houseColorVerts[0].m_color.GetAddress(), HOUSE_VERTEX_COUNT, ColorVertex::GetStride() / sizeof(GLfloat));
		houseMesh = Mesh(HOUSE_VERTEX_COUNT, HOUSE_INDEX_COUNT, &houseColorVerts[0], &houseIndices[0], GL_TRIANGLES, IndexSizeInBytes::Uint, s_PCShaderID, VertexFormat::PositionColor);

		if (!RenderEngine::AddMesh(&houseMesh))
		{
			GameLogger::Log(MessageType::cFatal_Error, "Failed to initialize ShapeGenerator! Could not add house mesh to render engine!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully setup house mesh!\n");
		return true;
	}

	bool ShapeGenerator::SetupDebugCube(Vec3 /*color*/)
	{
		debugCubeMesh = Mesh(DEBUG_CUBE_VERTEX_COUNT, DEBUG_CUBE_INDEX_COUNT, &debugCubeVerts[0], &debugCubeIndices[0], GL_LINES, IndexSizeInBytes::Uint, s_PShaderID, VertexFormat::PositionOnly);

		if (!RenderEngine::AddMesh(&debugCubeMesh))
		{
			GameLogger::Log(MessageType::cError, "Could not add debug cube mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully setup debug cube mesh!\n");
		return true;
	}

	bool ShapeGenerator::SetupDebugArrow(Vec3 topColor, Vec3 sideColor)
	{
		debugArrowMesh = Mesh(DEBUG_ARROW_VERTEX_COUNT, DEBUG_ARROW_INDEX_COUNT, &debugArrowColorVerts[0], &debugArrowIndices[0], GL_TRIANGLES, IndexSizeInBytes::Uint, s_PCShaderID, VertexFormat::PositionColor);
		SetColors(debugArrowColorVerts[0].m_color.GetAddress(), DEBUG_ARROW_SPLIT_VERTEX, debugArrowColorVerts[0].GetStride() / sizeof(GLfloat), topColor);
		SetColors(debugArrowColorVerts[DEBUG_ARROW_SPLIT_VERTEX].m_color.GetAddress(), DEBUG_ARROW_SPLIT_VERTEX, debugArrowColorVerts[0].GetStride() / sizeof(GLfloat), sideColor);

		if (!RenderEngine::AddMesh(&debugArrowMesh))
		{
			GameLogger::Log(MessageType::cError, "Could not add debug arrow mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully setup debug arrow mesh!\n");
		return true;
	}

	Mesh * ShapeGenerator::FindMeshBySceneString(const char * const sceneName, bool cull, unsigned int shaderId)
	{
		for (int i = 0; i < s_nextSceneFile; ++i)
		{
			if (StringFuncs::StringsAreEqual(sceneName, s_sceneFileNames[i]) && (cull == s_sceneMeshes[i]->IsCullingEnabledForObject()) && (shaderId == s_sceneMeshes[i]->GetShaderProgramID()))
			{
				return s_sceneMeshes[i];
			}
		}

		// indicate mesh does not exist
		return nullptr;
	}

	bool ShapeGenerator::AddMesh(const char * const sceneName, Mesh * pMeshToAdd)
	{
		// error checking
		if (FindMeshBySceneString(sceneName, pMeshToAdd->IsCullingEnabledForObject(), pMeshToAdd->GetShaderProgramID())) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to AddMesh([%s],[%p])! Scene already exists!\n", sceneName, pMeshToAdd); return false; }
		if (s_nextSceneFile >= MAX_SCENE_FILES) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to AddMesh([%s],[%p])! Ran out of space!\n", sceneName, pMeshToAdd); return false; }
		if (!pMeshToAdd) { GameLogger::Log(MessageType::cError, "ShapeGenerator failed to AddMesh([%s],[%p])! pMeshToAdd was nullptr!\n", sceneName, pMeshToAdd); return false; }

		// add to arrays
		s_sceneFileNames[s_nextSceneFile] = sceneName;
		s_sceneMeshes[s_nextSceneFile++] = pMeshToAdd;

		// indicate success
		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully added mesh #%d with name [%s] to list of scene files", s_nextSceneFile - 1, sceneName);
		return true;
	}

	bool ShapeGenerator::SetupSceneFile(const char * fileName, GLint shaderProgramID, const char *texturePath, bool cull)
	{
		// create binary reading input file stream
		std::ifstream inputStream(fileName, std::ios::binary | std::ios::in);

		// make sure its good
		if (!inputStream)
		{
			// log message
			GameLogger::Log(MessageType::cError, "ShapeGenerator failed to read scene file [%s], input stream is no good!\n", fileName);

			// indicate failure
			return false;
		}

		// variable to hold length in bytes to read in
		int dataLen = 0;

		// read in data length, which is the first piece of data in the custom format
		inputStream.read(reinterpret_cast<char*>(&dataLen), sizeof(dataLen));

		// Error checking to make sure length was read properly
		if (dataLen <= 0)
		{
			// log message
			GameLogger::Log(MessageType::cError, "ShapeGenerator failed to read scene file [%s], data len was invalid [%d]!\n", fileName, dataLen);

			// close stream
			inputStream.close();

			// indicate failure
			return false;
		}

		// allocate space for size specified in file
		char *data = new char[dataLen];

		// validate data allocated properly
		if (!data)
		{
			// log message
			GameLogger::Log(MessageType::cError, "ShapeGenerator failed to read scene file [%s]. Failed to allocate [%d] bytes of memory!\n", fileName, dataLen);

			// close file stream
			inputStream.close();

			// indicate failure
			return false;
		}

		// zero out newly allocated memory
		memset(data, 0, dataLen);

		// read in file stream
		inputStream.read(data, dataLen);

		// close file stream
		inputStream.close();

		// Convert data read to mesh
		Mesh *pSceneMesh = reinterpret_cast<Mesh*>(data);

		// get pointer to data vertices
		// vertices stored right after mesh
		char *pVertices = data + sizeof(*pSceneMesh);

		// get pointer to indices
		// indices stored right after vertices
		char *pIndices = pVertices + (pSceneMesh->GetSizeOfVertex()*pSceneMesh->GetVertexCount());

		// TODO: remove hard coded ColorVertex*s here!!!!!!
		*pSceneMesh = Mesh(pSceneMesh->GetVertexCount(), pSceneMesh->GetIndexCount(), pVertices, pIndices, pSceneMesh->GetMeshMode(), IndexSizeInBytes::Uint, shaderProgramID, pSceneMesh->GetVertexFormat(), cull);

		// if the mesh should be textured texture it
		if (pSceneMesh->GetVertexFormat() & VertexFormat::HasTexture && texturePath)
		{
			// if missing path fail
			//if (!texturePath)
			//{
			//	GameLogger::Log(MessageType::cError, "ShapeGenerator failed to read scene file [%s]! Mesh should be textured but no texture was passed in!\n", fileName);
			//	return false;
			//}

			// load texture
			int texID = BitmapLoader::LoadTexture(texturePath);

			// check if failure
			if (!texID)
			{
				GameLogger::Log(MessageType::cError, "ShapeGenerator failed to read scene file [%s]! Mesh should be textured but the texture [%s] could not be loaded!\n", fileName, texturePath);
				return false;
			}

			// if success pass id
			pSceneMesh->SetTextureID(texID);
		}

		if (!RenderEngine::AddMesh(pSceneMesh))
		{
			// log messsage
			GameLogger::Log(MessageType::cError, "ShapeGenerator failed to read scene file [%s], failed to add indexed mesh!\n", fileName);

			// indicate failure
			return false;
		}

		if (!AddMesh(fileName, pSceneMesh))
		{
			GameLogger::Log(MessageType::cError, "ShapeGenerator failed to read scene file [%s], failed to add mesh to list!\n", fileName);
			return false;
		}


		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully set up scene file [%s]!\n", fileName);
		return true;
	}

	bool ShapeGenerator::SetupSphereMesh(Vec3 color)
	{
		sphereMesh = SetupSphere(color);

		if (!RenderEngine::AddMesh(&sphereMesh))
		{
			GameLogger::Log(MessageType::cError, "Could not add debug sphere mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully setup sphere mesh!\n");
		return true;
	}

	Vec3 ShapeGenerator::frustumVerts[FRUSTUM_VERTEX_COUNT];

	GLuint ShapeGenerator::frustumIndices[FRUSTUM_INDEX_COUNT] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
		//0, 1, 5, 5, 0, 4,
		//1, 2, 5, 5, 2, 6,
		//2, 3, 6, 6, 3, 7,
		//3, 0, 7, 7, 0, 4
	};

	bool ShapeGenerator::SetupFrustum(float near, float far, float aspect, float fovy)
	{
		float nearh = tanf(fovy / 2.0f) * near;
		float nearw = nearh * aspect;
		float farh = tanf(fovy / 2.0f) * far;
		float farw = farh * aspect;

		frustumVerts[0] = Vec3(-nearw, +nearh, near);
		frustumVerts[1] = Vec3(+nearw, +nearh, near);
		frustumVerts[2] = Vec3(+nearw, -nearh, near);
		frustumVerts[3] = Vec3(-nearw, -nearh, near);
		frustumVerts[4] = Vec3(-farw, +farh, far);
		frustumVerts[5] = Vec3(+farw, +farh, far);
		frustumVerts[6] = Vec3(+farw, -farh, far);
		frustumVerts[7] = Vec3(-farw, -farh, far);

		frustumMesh = Mesh(FRUSTUM_VERTEX_COUNT, FRUSTUM_INDEX_COUNT, &frustumVerts[0], &frustumIndices[0], GL_LINES, IndexSizeInBytes::Uint, s_PShaderID, VertexFormat::PositionOnly, false);

		if (!RenderEngine::AddMesh(&frustumMesh))
		{
			GameLogger::Log(MessageType::cError, "Could not add debug cube mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully setup debug cube mesh!\n");
		return true;
	}

	bool ShapeGenerator::SetupHorizontalPlane(Vec3 upperLeft, Vec3 lowerRight)
	{
		planeNormalVerts[0] = ColorVertex{ upperLeft, Vec3(0.0f, 1.0f, 0.0f) };
		planeNormalVerts[1] = ColorVertex{ lowerRight, Vec3(0.0f, 1.0f, 0.0f) };
		planeNormalVerts[2] = ColorVertex{ Vec3(lowerRight.GetX(), upperLeft.GetY(), upperLeft.GetZ()), Vec3(0.0f, 1.0f, 0.0f) };
		planeNormalVerts[3] = ColorVertex{ upperLeft, Vec3(0.0f, 1.0f, 0.0f) };
		planeNormalVerts[4] = ColorVertex{ Vec3(upperLeft.GetX(), upperLeft.GetY(), lowerRight.GetZ()), Vec3(0.0f, 1.0f, 0.0f) };
		planeNormalVerts[5] = ColorVertex{ lowerRight, Vec3(0.0f, 1.0f, 0.0f) };

		horizontalPlaneMesh = Mesh(PLANE_VERTEX_COUNT, 0, &planeNormalVerts[0], nullptr, GL_TRIANGLES, IndexSizeInBytes::Uint, s_PNShaderID, VertexFormat::PositionNormal);

		if (!RenderEngine::AddMesh(&horizontalPlaneMesh))
		{
			GameLogger::Log(MessageType::cError, "Could not add plane mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully made a plane!\n");
		return true;
	}

	bool ShapeGenerator::SetupTessalatedPlane(Vec3 upperLeft, Vec3 lowerRight, Vec3 numTessalations, int shaderId)
	{
		int xCount = (int)numTessalations.GetX();
		int zCount = (int)numTessalations.GetZ();
		int numVertices = 2 * 3 * xCount * zCount;
		Vec3 *pVerts = new Vec3[numVertices]{ 0.0f };

		float minX = fminf(upperLeft.GetX(), lowerRight.GetX());
		float maxX = fmaxf(upperLeft.GetX(), lowerRight.GetX());
		float xRange = maxX - minX;
		float minZ = fminf(upperLeft.GetZ(), lowerRight.GetZ());
		float maxZ = fmaxf(upperLeft.GetZ(), lowerRight.GetZ());
		float zRange = maxZ - minZ;
		float oneXStep = xRange/xCount;
		float oneZStep = zRange/zCount;

		for (int x = 0; x < xCount; ++x)
		{
			for (int z = 0; z < zCount; ++z)
			{
				int i = 6 * (x + (xCount*z));
				float xPerc = x*(1.0f / xCount); // get how much, 0-1 to go in x
				float zPerc = z*(1.0f / zCount); // get how much, 0-1 to go in z
				float baseX = minX*xPerc + maxX*(1.0f - xPerc); // lerp from min to max for x
				float baseZ = minZ*zPerc + maxZ*(1.0f - zPerc); // lerp from min to max for z

				// make triangles for each
				pVerts[i + 0] = Vec3(baseX, 0.0f, baseZ);
				pVerts[i + 1] = Vec3(baseX + oneXStep, 0.0f, baseZ + oneZStep);
				pVerts[i + 2] = Vec3(baseX + oneXStep, 0.0f, baseZ);
				pVerts[i + 3] = Vec3(baseX, 0.0f, baseZ);
				pVerts[i + 4] = Vec3(baseX, 0.0f, baseZ + oneZStep);
				pVerts[i + 5] = Vec3(baseX + oneXStep, 0.0f, baseZ + oneZStep);
			}
		}

		s_pTessalatedPlanes[s_nextTessalatedPlane] = new Mesh(numVertices, 0, pVerts, nullptr, GL_TRIANGLES, IndexSizeInBytes::Uint, shaderId, VertexFormat::HasPosition);

		if (!RenderEngine::AddMesh(s_pTessalatedPlanes[s_nextTessalatedPlane]))
		{
			GameLogger::Log(MessageType::cError, "Could not add plane mesh!\n");
			return false;
		}

		++s_nextTessalatedPlane;

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully made a plane!\n");
		return true;
	}

	bool ShapeGenerator::SetupGridMesh(unsigned int widthInLines, unsigned int heightInLines, Vec3 /*color*/)
	{
		if (widthInLines > MAX_GRID_SIZE) { widthInLines = MAX_GRID_SIZE; }
		if (heightInLines > MAX_GRID_SIZE) { heightInLines = MAX_GRID_SIZE; }

		int numVertices = (widthInLines + 1) * 2 + (heightInLines + 1) * 2;

		gridMesh = Mesh(numVertices, 0, &gridVerts[0], nullptr, GL_LINES, IndexSizeInBytes::Uint, s_PShaderID, VertexFormat::PositionOnly);
		CreateGridMesh(widthInLines, heightInLines);

		if (!RenderEngine::AddMesh(&gridMesh))
		{
			GameLogger::Log(MessageType::cError, "Could not add grid mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully made a grid!\n");
		return true;
	}

	Vec3 ShapeGenerator::nearPlanePlaneVerts[NEARPLANENDC_VERTEX_COUNT] = {
		Vec3(-1.0f, 1.0f, 0.0f),
		Vec3(1.0f, 1.0f, 0.0f),
		Vec3(1.0f, -1.0f, 0.0f),
		Vec3(-1.0f, -1.0f, 0.0f)
	};

	GLuint ShapeGenerator::nearPlanePlaneIndices[NEARPLANENDC_INDEX_COUNT] = {
		0, 3, 1, 1, 3, 2 
	};

	bool ShapeGenerator::SetupNearPlanePlane(GLuint shaderProgramId)
	{
		nearPlaneMeshNDC = Mesh(NEARPLANENDC_VERTEX_COUNT, NEARPLANENDC_INDEX_COUNT, &nearPlanePlaneVerts[0], &nearPlanePlaneIndices[0], GL_TRIANGLES, IndexSizeInBytes::Uint, shaderProgramId, VertexFormat::PositionOnly, true);

		if (!RenderEngine::AddMesh(&nearPlaneMeshNDC))
		{
			GameLogger::Log(MessageType::cError, "Could not add plane mesh!\n");
			return false;
		}

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully made a plane!\n");
		return true;
	}
	
	ColorVertex ShapeGenerator::demoQuadVerts[DEMOQUAD_VERTEX_COUNT] = {
		Vec3{-0.05f, +0.05f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f},
		Vec3{ -0.05f, -0.05f, 0.0f }, Vec3{ 0.0f, 0.0f, 1.0f },
		Vec3{ +0.05f, -0.05f, 0.0f }, Vec3{ 0.0f, 1.0f, 0.0f },

		Vec3{-0.05f, +0.05f, 0.0f}, Vec3{1.0f, 0.0f, 0.0f},
		Vec3{+0.05f, -0.05f, 0.0f}, Vec3{0.0f, 1.0f, 0.0f},
		Vec3{+0.05f, +0.05f, 0.0f}, Vec3{0.0f, 1.0f, 1.0f}
	};

	bool ShapeGenerator::SetupDemoQuad(GLuint shaderProgramID)
	{
		s_demoQuadMeshes[s_nextQuadMesh] = new Mesh(DEMOQUAD_VERTEX_COUNT, 0, &demoQuadVerts[0], nullptr, GL_TRIANGLES, IndexSizeInBytes::Uint, shaderProgramID, VertexFormat::PositionColor, true);

		if (!RenderEngine::AddMesh(s_demoQuadMeshes[s_nextQuadMesh]))
		{
			GameLogger::Log(MessageType::cError, "Could not add DemoQuad mesh!\n");
			return false;
		}

		++s_nextQuadMesh;

		GameLogger::Log(MessageType::Process, "ShapeGenerator successfully made a DemoQuad!\n");
		return true;
	}

	//--------------------------------------------------------------
	// Sphere stuff
	// Code Below from supplementary lab doc, modifed to work
	//--------------------------------------------------------------

	enum SphereDefines
	{
		NUMSPHEREITERATIONS = 3,  // if you change this, you must change the next one too!!!
		FOUR_TO_NUM_ITERATIONS = 64,     // 4^numIterations, ie, 4 to the power of
		NUMSPHEREFACETS = FOUR_TO_NUM_ITERATIONS * 8, // 4^numIterations * 8
		NUMSPHEREVERTS = 3 * NUMSPHEREFACETS, // 3 verts per facet/triangle
		NUMSPHEREINDICES = 2 * NUMSPHEREVERTS   // two indices per point
	};

	Vertex sphereVerts[NUMSPHEREVERTS];
	Facet3      sphereFacets[NUMSPHEREFACETS];
	GLuint    sphereIndices[NUMSPHEREINDICES];

	Mesh ShapeGenerator::SetupSphere(Vec3 color)
	{
		BuildSphere(color);
		Mesh result;
		result = Mesh(NUMSPHEREVERTS, NUMSPHEREINDICES, sphereVerts, sphereIndices, GL_LINES, IndexSizeInBytes::Uint, s_PShaderID, VertexFormat::PositionOnly);

		return result;
	}

	void ShapeGenerator::BuildSphere(Vec3 /*color*/)
	{
		GenerateSphere(sphereFacets, NUMSPHEREITERATIONS);
		for (GLuint j = 0; j < NUMSPHEREFACETS; ++j)
		{
			sphereVerts[3 * j + 0].m_position = sphereFacets[j].p1;
			sphereVerts[3 * j + 1].m_position = sphereFacets[j].p2;
			sphereVerts[3 * j + 2].m_position = sphereFacets[j].p3;
			sphereIndices[6 * j + 0] = 3 * j + 0;
			sphereIndices[6 * j + 1] = 3 * j + 1;
			sphereIndices[6 * j + 2] = 3 * j + 1;
			sphereIndices[6 * j + 3] = 3 * j + 2;
			sphereIndices[6 * j + 4] = 3 * j + 2;
			sphereIndices[6 * j + 5] = 3 * j + 0;
		}
	}

	/* Create a triangular facet approximation to a sphere
	Return the number of facets created.
	The number of facets will be (4^iterations) * 8
	*/
	int ShapeGenerator::GenerateSphere(Facet3* facets, int iterations)
	{
		Vec3 p[6] =
		{
			Vec3(+0, +0, +1),
			Vec3(+0, +0, -1),
			Vec3(-1, -1, +0),
			Vec3(+1, -1, +0),
			Vec3(+1, +1, +0),
			Vec3(-1, +1, +0)
		};
		Vec3 pa, pb, pc;
		int numFacets = 0;
		int ntold;

		/* Create the level 0 object */
		float a = (float)(1 / sqrt(2.0));
		for (int i = 0; i < 6; i++)
		{
			p[i] = Vec3(p[i].GetX() * a, p[i].GetY() * a, p[i].GetZ());
		}

		facets[0].p1 = p[0];
		facets[0].p2 = p[3];
		facets[0].p3 = p[4];
		facets[1].p1 = p[0];
		facets[1].p2 = p[4];
		facets[1].p3 = p[5];
		facets[2].p1 = p[0];
		facets[2].p2 = p[5];
		facets[2].p3 = p[2];
		facets[3].p1 = p[0];
		facets[3].p2 = p[2];
		facets[3].p3 = p[3];
		facets[4].p1 = p[1];
		facets[4].p2 = p[4];
		facets[4].p3 = p[3];
		facets[5].p1 = p[1];
		facets[5].p2 = p[5];
		facets[5].p3 = p[4];
		facets[6].p1 = p[1];
		facets[6].p2 = p[2];
		facets[6].p3 = p[5];
		facets[7].p1 = p[1];
		facets[7].p2 = p[3];
		facets[7].p3 = p[2];
		numFacets = 8;
		if (iterations < 1) return numFacets;

		/* Bisect each edge and move to the surface of a unit sphere */
		for (int it = 0; it < iterations; it++)
		{
			ntold = numFacets;
			for (int i = 0; i < ntold; i++)
			{
				pa = Vec3((facets[i].p1.GetX() + facets[i].p2.GetX()) / 2, (facets[i].p1.GetY() + facets[i].p2.GetY()) / 2, (facets[i].p1.GetZ() + facets[i].p2.GetZ()) / 2).Normalize();
				pb = Vec3((facets[i].p2.GetX() + facets[i].p3.GetX()) / 2, (facets[i].p2.GetY() + facets[i].p3.GetY()) / 2, (facets[i].p2.GetZ() + facets[i].p3.GetZ()) / 2).Normalize();
				pc = Vec3((facets[i].p3.GetX() + facets[i].p1.GetX()) / 2, (facets[i].p3.GetY() + facets[i].p1.GetY()) / 2, (facets[i].p3.GetZ() + facets[i].p1.GetZ()) / 2).Normalize();

				facets[numFacets].p1 = facets[i].p1;
				facets[numFacets].p2 = pa;
				facets[numFacets].p3 = pc;

				numFacets++;
				facets[numFacets].p1 = pa;
				facets[numFacets].p2 = facets[i].p2;
				facets[numFacets].p3 = pb;

				numFacets++;
				facets[numFacets].p1 = pb;
				facets[numFacets].p2 = facets[i].p3;
				facets[numFacets].p3 = pc;

				numFacets++;
				facets[i].p1 = pa;
				facets[i].p2 = pb;
				facets[i].p3 = pc;
			}
		}

		return numFacets;
	}
}
