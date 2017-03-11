#ifndef COLLISIONTESTER_H
#define COLLISIONTESTER_H

#include "SpatialGrid.h"
#include "Vec3.h"
#include "ExportHeader.h"

// Justin Furtado
// 8/21/2016
// CollisionTester.h
// Does ray casting

namespace Engine
{
	class GraphicalObject;
	class Entity;

	struct ENGINE_SHARED RayCastingOutput
	{
		bool m_didIntersect{ false };
		float m_distance{ 999999.0f };
		Vec3 m_triangleNormal{ 0.0f, 0.0f, 0.0f };
		Vec3 m_alphaBetaGamma{ 0.0f, 0.0f, 0.0f };
		int m_vertexIndex{ -1 };
		GraphicalObject *m_belongsTo{ nullptr };
		Vec3 m_intersectionPoint{ 0.0f, 0.0f, 0.0f };
	};

	class ENGINE_SHARED CollisionTester
	{
	public:
		static void ConsoleLogOutput();
		static int GetGridWidthSections();
		static int GetGridHeightSections();
		static float GetGridScale();
		static RayCastingOutput FindWall(const Vec3& rayPosition, const Vec3& rayDirection, float checkDist);
		static RayCastingOutput FindWall(Entity *pEntity, float checkDist);
		static RayCastingOutput FindFloor(Entity *pEntity, float checkDist);
		static RayCastingOutput FindCeiling(Entity *pEntity, float checkDist);
		static RayCastingOutput RayTriangleIntersect(const Vec3& rayPosition, const Vec3& rayDirection, const Vec3& p0, const Vec3& p1, const Vec3& p2, float currentClosest);
		static bool AddGraphicalObject(GraphicalObject *pGraphicalObjectToAdd);
		static void RemoveGraphicalObject(GraphicalObject *pGobToRemove);
		static int GetTriangleCountForSpace(float xPos, float zPos);
		static int GetGridIndexFromPosX(float xPos);
		static int GetGridIndexFromPosZ(float zPos);
		static RayCastingOutput FindFromMousePos(int pixelX, int pixelY, float checkDist);
		static bool CalculateGrid();

	private:
		static GraphicalObject *s_pHeadNode;
		static SpatialGrid s_spatialGrid;

	};
}

#endif // ifndef COLLISIONTESTER_H