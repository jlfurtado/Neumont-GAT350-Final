#ifndef SHAPEGENERATOR_H
#define SHAPEGENERATOR_H

// Justin Furtado
// 6/28/2016
// ShapeGenerator.h
// Generates shapes

#include "ExportHeader.h"
#include "Mesh.h"
#include "Vertex.h"
#include "ColorVertex.h"
#include "Mat4.h"

namespace Engine
{
	enum class ShapeDataOption
	{
		INTERLEAVED,
		SEQUENTIAL,
		POSITION_ONLY,
		COLOR_ONLY,
		NUM_TYPES // This one last on purpose!
	};

	class GraphicalObject; // optimize - only need pointer
	typedef struct { Vec3 p1, p2, p3; } Facet3; // 3 points per Facet

	class ENGINE_SHARED ShapeGenerator
	{
	public:
		static bool Initialize(unsigned int pcShaderID, unsigned int pShaderID, unsigned int pnShaderID);
		static bool Shutdown();

		// methods
		static bool MakeFrustum(GraphicalObject *pObject, float near, float far, float aspect, float fovy);
		static bool MakeNormalCube(GraphicalObject *pObject);
		static bool MakeHorizontalPlane(GraphicalObject *pObject, Vec3 upperLeft, Vec3 lowerRight, Vec3 color);
		static bool MakeTessalatedPlane(GraphicalObject *pObject, Vec3 upperLeft, Vec3 lowerRight, Vec3 numTessalations, int shaderId);
		static bool MakeLightingCube(GraphicalObject *pObject);
		static bool MakeCube(GraphicalObject *pObject);
		static bool MakeTetrahedron(GraphicalObject *pObject);
		static bool MakeHouse(GraphicalObject *pObject);
		static bool MakeGrid(GraphicalObject *pObject, unsigned int width, unsigned int height, Vec3 color);
		static bool MakeDebugCube(GraphicalObject *pObject, Vec3 color);
		static bool MakeDebugArrow(GraphicalObject *pObject, Vec3 topColor, Vec3 sideColor);
		static bool MakeSphere(GraphicalObject *pObject, Vec3 color);
		static bool ReadSceneFile(const char *fileName, GraphicalObject *pObject, GLuint shaderProgramID, const char *texturePath = nullptr, bool cullObject = true);
		static bool CreatePoints(GraphicalObject *pObject, int numPoints, Vec3 upperLeftBound, Vec3 lowerRightBound, int shaderId, bool colorOn = false, Vec3 color = Vec3(0.0f));
		static bool CreatePoints(GraphicalObject *pObject, int numPoints, float thetaMin, float thetaMax, float phiMin, float phiMax, float velMin, float velMax, float spawnRate, int shaderId);
		static bool MakeNearPlanePlane(GraphicalObject * pObject, GLuint shaderProgramId);
		static bool MakeDemoQuad(GraphicalObject *pObject, GLuint shaderProgramID);

	private:
		// methods
		static const int MAX_SCENE_FILES = 30;
		static const int MAX_POINT_MESHES = 25;
		static const int MAX_TESSALATED_PLANES = 5;
		static const int MAX_QUAD_MESHES = 5;
		static const char *s_sceneFileNames[MAX_SCENE_FILES];
		static Mesh *s_sceneMeshes[MAX_SCENE_FILES];
		static int s_nextSceneFile;
		static Mesh *s_pPointMeshes[MAX_POINT_MESHES];
		static int s_nextPointMesh;
		static Mesh *s_pTessalatedPlanes[MAX_TESSALATED_PLANES];
		static int s_nextTessalatedPlane;
		static Vec3 s_numTesselations[MAX_TESSALATED_PLANES];
		static Mesh *s_demoQuadMeshes[MAX_QUAD_MESHES];
		static int s_nextQuadMesh;

		static Mesh *FindMeshBySceneString(const char *const sceneName, bool cull, unsigned int shaderId);
		static bool AddMesh(const char *const sceneName, Mesh *pMeshToAdd);
		static bool SetupSceneFile(const char *fileName, GLint shaderProgramID, const char *texturePath = nullptr, bool cull = true);
		static bool SetupSphereMesh(Vec3 color);
		static bool SetupFrustum(float near, float far, float aspect, float fovy);
		static bool SetupHorizontalPlane(Vec3 upperLeft, Vec3 lowerRight);
		static bool SetupTessalatedPlane(Vec3 upperLeft, Vec3 lowerRight, Vec3 numTessalations, int shaderId);
		static Mesh SetupSphere(Vec3 color);
		static int  GenerateSphere(Facet3* facets, int iterations);
		static void BuildSphere(Vec3 color);
		static void SetColors(float *color0, int numVerts, int stride);
		static void SetColors(float *color0, int numVerts, int stride, Vec3 color);
		static void CreateGridMesh(unsigned int widthInLines, unsigned int heightInLines);
		static bool SetupCubeMesh();
		static bool SetupLightingCubeMesh();
		static bool SetupNormalCube();
		static bool SetupTetrahedronMesh();
		static bool SetupHouseMesh();
		static bool SetupDebugCube(Vec3 color);
		static bool SetupDebugArrow(Vec3 topColor, Vec3 sideColor);
		static bool SetupGridMesh(unsigned int widthInLines, unsigned int heightInLines, Vec3 color);
		static bool SetupNearPlanePlane(GLuint shaderProgramId);
		static bool SetupDemoQuad(GLuint shaderProgramID);

		// data
		static unsigned int s_PCShaderID;
		static unsigned int s_PShaderID;
		static unsigned int s_PNShaderID;

		static const int CUBE_VERTEX_COUNT = 36;
		static ColorVertex cubeColorVerts[CUBE_VERTEX_COUNT];

		static const int NORMAL_CUBE_VERTEX_COUNT = 36;
		static ColorVertex cubeNormalVerts[NORMAL_CUBE_VERTEX_COUNT];

		static const int PLANE_VERTEX_COUNT = 6;
		static ColorVertex planeNormalVerts[PLANE_VERTEX_COUNT];

		static const int LIGHTING_CUBE_VERTEX_COUNT = 36;
		static Vertex lightingCubeVerts[LIGHTING_CUBE_VERTEX_COUNT];

		static const int TETRAHEDRON_VERTEX_COUNT = 4;
		static ColorVertex tetrahedronColorVerts[TETRAHEDRON_VERTEX_COUNT];
		static const int TETRAHEDRON_INDEX_COUNT = 12;
		static GLuint tetrahedronIndices[TETRAHEDRON_INDEX_COUNT];

		static const int HOUSE_VERTEX_COUNT = 24;
		static ColorVertex houseColorVerts[HOUSE_VERTEX_COUNT];
		static const int HOUSE_INDEX_COUNT = 42;
		static GLuint houseIndices[HOUSE_INDEX_COUNT];

		static const int MAX_GRID_SIZE = 10000;
		static const int MAX_GRID_VERTS = 4 * (MAX_GRID_SIZE + 1);
		static Vertex gridVerts[MAX_GRID_VERTS];

		static const int DEBUG_CUBE_VERTEX_COUNT = 8;
		static Vertex debugCubeVerts[DEBUG_CUBE_VERTEX_COUNT];
		static const int DEBUG_CUBE_INDEX_COUNT = 24;
		static GLuint debugCubeIndices[DEBUG_CUBE_INDEX_COUNT];

		// 9 quads and 2 tris
		static const int DEBUG_ARROW_VERTEX_COUNT = 28; // top and sides are different colors
		static ColorVertex debugArrowColorVerts[DEBUG_ARROW_VERTEX_COUNT];
		static const int DEBUG_ARROW_INDEX_COUNT = 60; // 9*6+2*3
		static GLuint debugArrowIndices[DEBUG_ARROW_INDEX_COUNT];
		static const int DEBUG_ARROW_SPLIT_VERTEX = 14; // 7 for top and bottom, then 14 for sides

		static const int FRUSTUM_VERTEX_COUNT = 8;
		static Vec3 frustumVerts[FRUSTUM_VERTEX_COUNT];
		static const int FRUSTUM_INDEX_COUNT = 24;
		static GLuint frustumIndices[FRUSTUM_INDEX_COUNT];

		static const int NEARPLANENDC_VERTEX_COUNT = 4;
		static Vec3 nearPlanePlaneVerts[NEARPLANENDC_VERTEX_COUNT];
		static const int NEARPLANENDC_INDEX_COUNT = 6;
		static GLuint nearPlanePlaneIndices[NEARPLANENDC_INDEX_COUNT];

		static const int DEMOQUAD_VERTEX_COUNT = 6;
		static ColorVertex demoQuadVerts[DEMOQUAD_VERTEX_COUNT];

		static Mesh debugCubeMesh;
		static Mesh gridMesh;
		static Mesh cubeMesh;
		static Mesh lightingCubeMesh;
		static Mesh tetrahedronMesh;
		static Mesh houseMesh;
		static Mesh debugArrowMesh;
		static Mesh sphereMesh;
		static Mesh horizontalPlaneMesh;
		static Mesh normalCubeMesh;
		static Mesh frustumMesh;
		static Mesh nearPlaneMeshNDC;
	};
}

#endif // ifndef SHAPEGENERATOR_H