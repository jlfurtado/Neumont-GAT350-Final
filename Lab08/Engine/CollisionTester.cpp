#include "CollisionTester.h"
#include "CollisionTester.h"
#include "CollisionTester.h"
#include "CollisionTester.h"
#include "GraphicalObject.h"
#include "Mesh.h"
#include "Entity.h"
#include "SpatialComponent.h"
#include "GameLogger.h"
#include "MousePicker.h"

// Justin Furtado
// 8/21/2016
// CollisionTester.h
// Does ray casting

namespace Engine
{
	GraphicalObject *CollisionTester::s_pHeadNode = nullptr;
	SpatialGrid CollisionTester::s_spatialGrid;

	void CollisionTester::ConsoleLogOutput()
	{
		s_spatialGrid.CalculateStatisticsFromCounts();
		s_spatialGrid.ConsoleLogStats();
	}

	int CollisionTester::GetGridWidthSections()
	{
		return s_spatialGrid.GetGridWidth();
	}

	int CollisionTester::GetGridHeightSections()
	{
		return s_spatialGrid.GetGridHeight();
	}

	float CollisionTester::GetGridScale()
	{
		return s_spatialGrid.GetGridScale();
	}

	RayCastingOutput CollisionTester::FindWall(const Vec3 & rayPosition, const Vec3 & rayDirection, float checkDist)
	{
		// normalize input for future ray casts
		Vec3 rd = rayDirection.Normalize();

		// create variable to hold output
		RayCastingOutput finalOutput;

		// grab scale and calculate end position
		float gridScale = s_spatialGrid.GetGridScale();
		Vec3 rp = rayPosition + Vec3(0.5f * gridScale* s_spatialGrid.GetGridWidth(), 0.0f, 0.5f*gridScale*s_spatialGrid.GetGridHeight());
		Vec3 endPosition = rp + rd*checkDist;

		/// calculate the begining and end grid indices from the positions
		int i = (int)floorf(rp.GetX() / gridScale);
		int j = (int)floorf(rp.GetZ() / gridScale);
		int iEnd = (int)floorf(endPosition.GetX() / gridScale);
		int jEnd = (int)floorf(endPosition.GetZ() / gridScale);

		// pick the directions to step in
		int deltaI = ((rp.GetX() < endPosition.GetX()) ? 1 : ((rp.GetX() > endPosition.GetX()) ? -1 : 0));
		int deltaJ = ((rp.GetZ() < endPosition.GetZ()) ? 1 : ((rp.GetZ() > endPosition.GetZ()) ? -1 : 0));

		// MATHS!!! 
		float minX = gridScale * floorf(rp.GetX() / gridScale);
		float maxX = minX + gridScale;
		float tx = ((rp.GetX() > endPosition.GetX()) ? (rp.GetX() - minX) : (maxX - rp.GetX())) / fabsf(endPosition.GetX() - rp.GetX());
		float minZ = gridScale * floorf(rp.GetZ() / gridScale);
		float maxZ = gridScale + minZ;
		float tz = ((rp.GetZ() > endPosition.GetZ()) ? (rp.GetZ() - minZ) : (maxZ - rp.GetZ())) / fabsf(endPosition.GetZ() - rp.GetZ());

		// calculate more things
		float dx = gridScale / fabsf(endPosition.GetX() - rp.GetX());
		float dz = gridScale / fabsf(endPosition.GetZ() - rp.GetZ());

		for (;;)
		{
			SpatialTriangleData *pFirst = s_spatialGrid.GetTriangleDataByGrid(i, j);

			for (int c = 0; c < s_spatialGrid.GetGridTriangleCount(i, j); ++c)
			{
				SpatialTriangleData *pCurrent = pFirst + c;
				if (pCurrent)
				{
					Mesh *pMesh = pCurrent->m_pTriangleOwner->GetMeshPointer();
					Mat4 modelToWorld = pCurrent->m_pTriangleOwner->GetTransMat() * (pCurrent->m_pTriangleOwner->GetScaleMat() * pCurrent->m_pTriangleOwner->GetRotMat());
					void *pVertices = pMesh->GetVertexPointer();
					int meshFormatSize = VertexFormatSize(pMesh->GetVertexFormat());

					if (pCurrent->m_pTriangleOwner->IsEnabled())
					{
						Vec3 p0 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * pCurrent->m_triangleVertexZeroIndex))));
						Vec3 p1 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (pCurrent->m_triangleVertexZeroIndex + 1)))));
						Vec3 p2 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (pCurrent->m_triangleVertexZeroIndex + 2)))));

						RayCastingOutput output = RayTriangleIntersect(rayPosition, rd, p0, p1, p2, finalOutput.m_distance);
						if (output.m_didIntersect && output.m_distance < finalOutput.m_distance) { finalOutput = output; finalOutput.m_belongsTo = pCurrent->m_pTriangleOwner; finalOutput.m_vertexIndex = pCurrent->m_triangleVertexZeroIndex; }
					}
				}
				
			}

			// checking further is irrelevant because a triangle has been hit in the closest partition
			//if (finalOutput.m_didIntersect) { return finalOutput; }
			// TODO: BETTER FIX FOR BUG WITH TRIANGLE COMPARISON ORDER


			if (tx <= tz)
			{
				if (i == iEnd) { break; }
				tx += dx;
				i += deltaI;
			}
			else
			{
				if (j == jEnd) { break; }
				tz += dz;
				j += deltaJ;
			}
		}

		return finalOutput;
	}

	RayCastingOutput CollisionTester::FindWall(Entity * pEntity, float checkDist)
	{
		if (!pEntity) { GameLogger::Log(MessageType::cError, "Failed to find wall for entity! Entity passed was nullptr!\n"); return RayCastingOutput(); }

		SpatialComponent *pSpatial = pEntity->GetComponentByType<SpatialComponent>();
		if (!pSpatial) { GameLogger::Log(MessageType::cError, "Failed to find wall for entity! Entity has no spatial component!\n"); return RayCastingOutput(); }

		return FindWall(pSpatial->GetPosition(), pSpatial->GetForward(), checkDist);
	}

	RayCastingOutput CollisionTester::FindFloor(Entity * pEntity, float checkDist)
	{
		if (!pEntity) { GameLogger::Log(MessageType::cError, "Failed to find floor for entity! Entity passed was nullptr!\n"); return RayCastingOutput(); }

		SpatialComponent *pSpatial = pEntity->GetComponentByType<SpatialComponent>();
		if (!pSpatial) { GameLogger::Log(MessageType::cError, "Failed to find floor for entity! Entity has no spatial component!\n"); return RayCastingOutput(); }

		return FindWall(pSpatial->GetPosition(), Vec3(0.0f, -1.0f, 0.0f), checkDist);
	}

	RayCastingOutput CollisionTester::FindCeiling(Entity * pEntity, float checkDist)
	{
		if (!pEntity) { GameLogger::Log(MessageType::cError, "Failed to find ceiling for entity! Entity passed was nullptr!\n"); return RayCastingOutput(); }

		SpatialComponent *pSpatial = pEntity->GetComponentByType<SpatialComponent>();
		if (!pSpatial) { GameLogger::Log(MessageType::cError, "Failed to find ceiling for entity! Entity has no spatial component!\n"); return RayCastingOutput(); }

		return FindWall(pSpatial->GetPosition(), Vec3(0.0f, 1.0f, 0.0f), checkDist);
	}

	RayCastingOutput CollisionTester::RayTriangleIntersect(const Vec3 & rayPosition, const Vec3 & rayDirection, const Vec3 & p0, const Vec3 & p1, const Vec3 & p2, float currentClosest)
	{
		// create output to be returned
		RayCastingOutput output;

		// calculate edge vectors
		Vec3 e0 = p1 - p0;
		Vec3 e1 = p2 - p1;

		// calculate normal of triangle
		Vec3 n = e0.Cross(e1).Normalize();

		// do maths
		float dot = n.Dot(rayDirection);

		// early-out - NOTE: DOES NOT BEHAVE THE SAME AS (dot >= 0.0f) BECAUSE OF NANS
		if (!(dot < 0.0f))
		{
			// no intersection because ray not facing triangle
			return output;
		}

		// plane maths
		float d = n.Dot(p0);

		// parametric math
		float t = d - n.Dot(rayPosition);

		// another early out, NOTE AGAIN BAIL FOR NANS
		if (!(t <= 0.0f))
		{
			// no interesction because ray not intersecting plane
			return output;
		}

		// yet another early out and, you guessed it, NANS
		if (!(t >= dot*currentClosest))
		{
			// no intersection because closer intersection already exists based on passed in parameter or ray does not intersect plane
			return output;
		}

		// calculate point of intersection and error check
		t /= dot;
		//if (!(t >= 0.0f)) { GameLogger::Log(MessageType::cFatal_Error, "t [%f] was not positive in ray casting... something is wrong!\n", t); }
		//if (!(t <= currentClosest)) { 
		//	GameLogger::Log(MessageType::cFatal_Error, "t [%f] was further than closest [%f] in ray casting... something is wrong!\n", t, currentClosest);
		//}

		// maths
		Vec3 p = rayPosition + rayDirection*t;

		output.m_intersectionPoint = p;

		float u0, u1, u2;
		float v0, v1, v2;
		if (fabs(n.GetX()) > fabs(n.GetY()))
		{
			if (fabs(n.GetX()) > fabs(n.GetZ()))
			{
				u0 = p.GetY() - p0.GetY();
				u1 = p1.GetY() - p0.GetY();
				u2 = p2.GetY() - p0.GetY();

				v0 = p.GetZ() - p0.GetZ();
				v1 = p1.GetZ() - p0.GetZ();
				v2 = p2.GetZ() - p0.GetZ();
			}
			else
			{
				u0 = p.GetX() - p0.GetX();
				u1 = p1.GetX() - p0.GetX();
				u2 = p2.GetX() - p0.GetX();

				v0 = p.GetY() - p0.GetY();
				v1 = p1.GetY() - p0.GetY();
				v2 = p2.GetY() - p0.GetY();
			}
		}
		else
		{
			if (fabs(n.GetY()) > fabs(n.GetZ()))
			{
				u0 = p.GetX() - p0.GetX();
				u1 = p1.GetX() - p0.GetX();
				u2 = p2.GetX() - p0.GetX();

				v0 = p.GetZ() - p0.GetZ();
				v1 = p1.GetZ() - p0.GetZ();
				v2 = p2.GetZ() - p0.GetZ();
			}
			else
			{
				u0 = p.GetX() - p0.GetX();
				u1 = p1.GetX() - p0.GetX();
				u2 = p2.GetX() - p0.GetX();

				v0 = p.GetY() - p0.GetY();
				v1 = p1.GetY() - p0.GetY();
				v2 = p2.GetY() - p0.GetY();
			}
		}

		// compute denominator, check for invalid
		float temp = u1 * v2 - v1 * u2;
		if (!(temp != 0.0f))
		{
			// invalid denominator out
			return output;
		}

		temp = 1.0f / temp;

		// compute barycentric coordinates, check out of range at each step

		float alpha = (u0*v2 - v0*u2)*temp;
		if (!(alpha >= 0.0f))
		{
			// alpha out of range out
			return output;
		}

		float beta = (u1 *v0 - v1*u0) * temp;
		if (!(beta >= 0.0f))
		{
			// beta out of range out
			return output;
		}

		float gamma = 1.0f - alpha - beta;
		if (!(gamma >= 0.0f))
		{
			// gamma out of range out
			return output;
		}

		// success, return values
		output.m_didIntersect = true;
		output.m_triangleNormal = n;
		output.m_distance = t;
		output.m_alphaBetaGamma = Engine::Vec3(alpha, beta, gamma);
		return output;
	}

	bool CollisionTester::AddGraphicalObject(GraphicalObject * pGraphicalObjectToAdd)
	{
		if (!pGraphicalObjectToAdd) { GameLogger::Log(MessageType::cError, "Failed to AddGraphicalObject to CollisionTester! GraphicalObject to-be-added was nullptr!\n"); return false; }
		if (pGraphicalObjectToAdd == s_pHeadNode) { GameLogger::Log(MessageType::cError, "Failed to AddGraphicalObject to CollisionTester! GrapicalObject to-be-added was the same as the head node! (Object would point to itself and list could not be iterated over)\n"); return false; }

		return s_spatialGrid.AddGraphicalObject(pGraphicalObjectToAdd);
	}

	void CollisionTester::RemoveGraphicalObject(GraphicalObject * pGobToRemove)
	{
		s_spatialGrid.RemoveGraphicalObject(pGobToRemove);
	}

	int CollisionTester::GetTriangleCountForSpace(float xPos, float zPos)
	{
		static int lastGridX = -1;
		static int lastGridZ = -1;
		int gridX = GetGridIndexFromPosX(xPos);
		int gridZ = GetGridIndexFromPosZ(zPos);
		if (lastGridX == gridX && gridZ == lastGridZ) { return -1; }
		else {
			return s_spatialGrid.GetGridTriangleCount(gridX, gridZ);
		}
	}

	int CollisionTester::GetGridIndexFromPosX(float xPos)
	{
		return  (int)floorf((xPos / s_spatialGrid.GetGridScale()) + (s_spatialGrid.GetGridWidth()*0.5f));
	}

	int CollisionTester::GetGridIndexFromPosZ(float zPos)
	{
		return (int)floorf((zPos / s_spatialGrid.GetGridScale()) + (s_spatialGrid.GetGridHeight()*0.5f));
	}

	RayCastingOutput CollisionTester::FindFromMousePos(int pixelX, int pixelY, float checkDist)
	{
		return FindWall(MousePicker::GetOrigin(pixelX, pixelY), MousePicker::GetDirection(pixelX, pixelY), checkDist);
	}

	bool CollisionTester::CalculateGrid()
	{
		return s_spatialGrid.AddTrianglesToPartitions();
	}

}

