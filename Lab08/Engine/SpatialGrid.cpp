#include "SpatialGrid.h"
#include "GameLogger.h"
#include "Mesh.h"
#include "GraphicalObject.h"
#include "Vec3.h"
#include "Vec4.h"

// Justin Furtado
// SpatialGrid.h
// 8/24/2016
// Holds an array of linked lists of SpatialTriangleData used for spatial partitioning

// TODO: Refactor out duplicate code	

namespace Engine
{
	const float SPATIAL_GRID_SIZE = 50.0f;
	SpatialGrid::SpatialGrid()
		: m_gridScale(SPATIAL_GRID_SIZE)
	{
	}

	SpatialGrid::~SpatialGrid()
	{
		CleanUp();
	}

	SpatialTriangleData * SpatialGrid::GetTriangleDataByGrid(int gridX, int gridZ)
	{
		if (gridX < 0 || gridX >= m_gridSectionsWidth || gridZ < 0 || gridZ >= m_gridSectionsHeight) { return nullptr; }
		int i = gridZ*m_gridSectionsWidth + gridX;
		//GameLogger::Log(MessageType::ConsoleOnly, "i = %d\n", i);
		return &m_pData[m_pGridStartIndices[i]];
	}

	SpatialTriangleData * SpatialGrid::GetTriangleDataByGridAtPosition(float worldX, float worldZ)
	{
		return GetTriangleDataByGrid(int(worldX / m_gridScale - 0.5f*m_gridSectionsWidth), int(worldZ / m_gridScale - 0.5f*m_gridSectionsHeight));
	}

	bool SpatialGrid::AddGraphicalObject(GraphicalObject * pGraphicalObjectToAdd)
	{
		m_graphicalObjectCount++;
		m_totalTriangleCount += pGraphicalObjectToAdd->GetMeshPointer()->GetVertexCount() / 3;
		pGraphicalObjectToAdd->PointToCollidingNode(m_pHeadNode);
		m_pHeadNode = pGraphicalObjectToAdd;
		return true;
	}

	float SpatialGrid::GetGridScale()
	{
		return m_gridScale;
	}

	int SpatialGrid::GetGridTriangleCount(int gridX, int gridZ)
	{
		if (gridX < 0 || gridX >= m_gridSectionsWidth || gridZ < 0 || gridZ >= m_gridSectionsHeight) { return -1; }
		int i = gridZ*m_gridSectionsWidth + gridX;

		return m_pGridTriangleCounts[i];
	}

	void SpatialGrid::CalculateStatisticsFromCounts()
	{
		m_totalTriangleCount = 0;

		for (int i = 0; i < m_totalGridSections; ++i)
		{
			if (m_pGridTriangleCounts[i] < m_minGridTriangleCount || m_minGridTriangleCount < 0)
			{
				m_minGridTriangleCount = m_pGridTriangleCounts[i];
			}

			if (m_pGridTriangleCounts[i] > m_maxGridTriangleCount || m_maxGridTriangleCount < 0)
			{
				m_maxGridTriangleCount = m_pGridTriangleCounts[i];
			}

			m_totalTriangleCount += m_pGridTriangleCounts[i];
		}

		m_avgGridTriangleCount = (float)m_totalTriangleCount / (float)m_totalGridSections;
	}

	void SpatialGrid::RemoveGraphicalObject(GraphicalObject * pGobToRemove)
	{
		GraphicalObject *pCurrentNode = m_pHeadNode;
		if (m_pHeadNode == pGobToRemove) { m_pHeadNode = m_pHeadNode->GetNextCollidingNode(); return; }

		while (pCurrentNode)
		{
			if (pCurrentNode->GetNextCollidingNode() == pGobToRemove) { pCurrentNode->PointToCollidingNode(pGobToRemove->GetNextCollidingNode()); return; }
			pCurrentNode = pCurrentNode->GetNextCollidingNode();
		}
	}

	int SpatialGrid::GetGridWidth()
	{
		return m_gridSectionsWidth;
	}

	int SpatialGrid::GetGridHeight()
	{
		return m_gridSectionsHeight;
	}

	void SpatialGrid::ConsoleLogStats()
	{
		GameLogger::Log(MessageType::ConsoleOnly, "Total triangle count for grid is [%d]\n", m_totalTriangleCount);
		GameLogger::Log(MessageType::ConsoleOnly, "Average triangle count for grid cells is [%.3f]\n", m_avgGridTriangleCount);
		GameLogger::Log(MessageType::ConsoleOnly, "Min triangle count for grid is [%d]\n", m_minGridTriangleCount);
		GameLogger::Log(MessageType::ConsoleOnly, "Max triangle count for grid is [%d]\n", m_maxGridTriangleCount);
	}

	bool SpatialGrid::AddTrianglesToPartitions()
	{
		static bool first = true;

		// allow method to be called repeatedly
		if (!first) { CleanUp(); }
		else { first = false; }

		m_pGridStartIndices = new int[m_totalGridSections] {0};
		m_pGridTriangleCounts = new int[m_totalGridSections] {0};
		if (!m_pGridStartIndices) { GameLogger::Log(MessageType::cFatal_Error, "Failed to allocate memory for [%d] ints!\n", m_totalGridSections); return false; }
		if (!m_pGridTriangleCounts) { GameLogger::Log(MessageType::cFatal_Error, "Failed to allocate memory for [%d] ints!\n", m_totalGridSections); return false; }

		if (!SetGridStartIndices()) { GameLogger::Log(MessageType::cFatal_Error, "Failed to set grid start indices!\n"); return false; }

		CalculateStatisticsFromCounts();
		m_pData = new SpatialTriangleData[m_totalTriangleCount];
		if (!m_pData) { GameLogger::Log(MessageType::cFatal_Error, "Failed to allocate memory for [%d] triangles!\n", m_totalTriangleCount); return false; }

		for (int i = 0; i < m_totalGridSections; ++i)
		{
			m_pGridTriangleCounts[i] = 0;
		}


		GraphicalObject *pCurrentNode = m_pHeadNode;
		while (pCurrentNode)
		{
			if (!AddGraphicalObjectToGrid(pCurrentNode)) { GameLogger::Log(MessageType::cFatal_Error, "Failed to make spatial grid! Failed to add GraphicalObjectToGrid()!\n"); }
			pCurrentNode = pCurrentNode->GetNextCollidingNode();
		}

		GameLogger::Log(MessageType::Process, "Successfully re-calculated spatial grid!\n");
		return true;
	}

	bool SpatialGrid::SetGridStartIndices()
	{
		GraphicalObject *pCurrentNode = m_pHeadNode;
		while (pCurrentNode)
		{
			// model to world matrix
			Mat4 modelToWorld = pCurrentNode->GetTransMat() * (pCurrentNode->GetScaleMat() * pCurrentNode->GetRotMat());

			// convenience data
			Mesh *pMesh = pCurrentNode->GetMeshPointer();
			int meshFormatSize = VertexFormatSize(pMesh->GetVertexFormat());
			void *pVertices = pMesh->GetVertexPointer();

			// loop through every triangle
			if (pMesh->IsIndexed())
			{
				GLuint *pIndices = (GLuint *)pMesh->GetIndexPointer();
				for (unsigned int i = 0; i < pMesh->GetIndexCount(); i += 3)
				{
					Vec3 p0 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * pIndices[i + 0]))));
					Vec3 p1 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * pIndices[i + 1]))));
					Vec3 p2 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * pIndices[i + 2]))));

					// extract the leftmost, upmost, downmost, rightmost grid indices for which the bounding box of the triangle enters
					// TODO: investigate the case where a triangle is added to a grid in which the bounding quad intersects but the triangle does not
					// NOTE: if the grid size is larger than the triangle size, the triangle can be in at most four grid spaces, and the above TODO: would only have a small effect
					float offsetX = 0.5f*m_gridSectionsWidth;
					float offsetZ = 0.5f*m_gridSectionsHeight;
					int minGridX = (int)fminf(fminf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
					int minGridZ = (int)fminf(fminf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);
					int maxGridX = (int)fmaxf(fmaxf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
					int maxGridZ = (int)fmaxf(fmaxf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);

					// error checking
					if (minGridX < 0 || minGridX > m_gridSectionsWidth || minGridZ < 0 || minGridZ > m_gridSectionsHeight || maxGridX < 0 || maxGridX > m_gridSectionsWidth || maxGridZ < 0 || maxGridZ > m_gridSectionsHeight)
					{
						GameLogger::Log(MessageType::cWarning, "Tried to AddGraphicalObject to SpatialGrid but some triangles were out of grid range!\n");
						return false;
					}

					// add to all grid cells in range
					for (int x = minGridX; x <= maxGridX; ++x)
					{
						for (int z = minGridZ; z <= maxGridZ; ++z)
						{
							m_pGridTriangleCounts[z*m_gridSectionsWidth + x]++;
						}
					}
				}
			}
			else
			{
				for (unsigned int i = 0; i < pCurrentNode->GetMeshPointer()->GetVertexCount(); i += 3)
				{
					// grab the vertex positions regardless of format
					Vec3 p0 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (i + 0)))));
					Vec3 p1 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (i + 1)))));
					Vec3 p2 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (i + 2)))));

					// extract the leftmost, upmost, downmost, rightmost grid indices for which the bounding box of the triangle enters
					// TODO: investigate the case where a triangle is added to a grid in which the bounding quad intersects but the triangle does not
					// NOTE: if the grid size is larger than the triangle size, the triangle can be in at most four grid spaces, and the above would only have a small effect
					float offsetX = 0.5f*m_gridSectionsWidth;
					float offsetZ = 0.5f*m_gridSectionsHeight;
					int minGridX = (int)fminf(fminf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
					int minGridZ = (int)fminf(fminf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);
					int maxGridX = (int)fmaxf(fmaxf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
					int maxGridZ = (int)fmaxf(fmaxf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);

					// error checking
					if (minGridX < 0 || minGridX > m_gridSectionsWidth || minGridZ < 0 || minGridZ > m_gridSectionsHeight || maxGridX < 0 || maxGridX > m_gridSectionsWidth || maxGridZ < 0 || maxGridZ > m_gridSectionsHeight)
					{
						GameLogger::Log(MessageType::cWarning, "Tried to AddGraphicalObject to SpatialGrid but some triangles were out of grid range!\n");
						return false;
					}

					// add to all grid cells in range
					for (int x = minGridX; x <= maxGridX; ++x)
					{
						for (int z = minGridZ; z <= maxGridZ; ++z)
						{
							m_pGridTriangleCounts[z*m_gridSectionsWidth + x]++;
						}
					}
				}
			}

			pCurrentNode = pCurrentNode->GetNextCollidingNode();
		}

		int c = 0;
		for (int x = 0; x < m_gridSectionsWidth; ++x)
		{
			for (int z = 0; z < m_gridSectionsHeight; ++z)
			{
				m_pGridStartIndices[z*m_gridSectionsWidth + x] = c;
				c += m_pGridTriangleCounts[z*m_gridSectionsWidth + x];
			}
		}

		// indicate success
		GameLogger::Log(MessageType::Process, "Successfully added GraphicalObject to SpatialGrid!\n");
		return true;
	}	
	
	bool SpatialGrid::AddGraphicalObjectToGrid(GraphicalObject * pGraphicalObjectToAdd)
	{
		// model to world matrix
		Mat4 modelToWorld = pGraphicalObjectToAdd->GetTransMat() * (pGraphicalObjectToAdd->GetScaleMat() * pGraphicalObjectToAdd->GetRotMat());

		// convenience data
		Mesh *pMesh = pGraphicalObjectToAdd->GetMeshPointer();
		int meshFormatSize = VertexFormatSize(pMesh->GetVertexFormat());
		void *pVertices = pMesh->GetVertexPointer();

		// loop through every triangle
		if (pMesh->IsIndexed())
		{
			GLuint *pIndices = (GLuint *)pMesh->GetIndexPointer();
			for (unsigned int i = 0; i < pMesh->GetIndexCount(); i += 3)
			{
				Vec3 p0 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * pIndices[i + 0]))));
				Vec3 p1 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * pIndices[i + 1]))));
				Vec3 p2 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * pIndices[i + 2]))));

				// extract the leftmost, upmost, downmost, rightmost grid indices for which the bounding box of the triangle enters
				// TODO: investigate the case where a triangle is added to a grid in which the bounding quad intersects but the triangle does not
				// NOTE: if the grid size is larger than the triangle size, the triangle can be in at most four grid spaces, and the above TODO: would only have a small effect
				float offsetX = 0.5f*m_gridSectionsWidth;
				float offsetZ = 0.5f*m_gridSectionsHeight;
				int minGridX = (int)fminf(fminf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
				int minGridZ = (int)fminf(fminf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);
				int maxGridX = (int)fmaxf(fmaxf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
				int maxGridZ = (int)fmaxf(fmaxf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);

				// error checking
				if (minGridX < 0 || minGridX > m_gridSectionsWidth || minGridZ < 0 || minGridZ > m_gridSectionsHeight || maxGridX < 0 || maxGridX > m_gridSectionsWidth || maxGridZ < 0 || maxGridZ > m_gridSectionsHeight)
				{
					GameLogger::Log(MessageType::cWarning, "Tried to AddGraphicalObject to SpatialGrid but some triangles were out of grid range!\n");
					return false;
				}

				// add to all grid cells in range
				for (int x = minGridX; x <= maxGridX; ++x)
				{
					for (int z = minGridZ; z <= maxGridZ; ++z)
					{
						SpatialTriangleData newData;
						newData.m_pTriangleOwner = pGraphicalObjectToAdd;
						newData.m_triangleVertexZeroIndex = pIndices[i]; // pIndices[i] is index of p0
						int w = m_pGridStartIndices[z*m_gridSectionsWidth + x] + m_pGridTriangleCounts[z*m_gridSectionsWidth + x];
						m_pData[w] = newData;
						m_pGridTriangleCounts[z*m_gridSectionsWidth + x]++;
					}
				}
			}
		}
		else
		{
			for (unsigned int i = 0; i < pGraphicalObjectToAdd->GetMeshPointer()->GetVertexCount(); i += 3)
			{
				// grab the vertex positions regardless of format
				Vec3 p0 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (i + 0)))));
				Vec3 p1 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (i + 1)))));
				Vec3 p2 = modelToWorld * (*(reinterpret_cast<Vec3 *>(reinterpret_cast<char *>(pVertices) + (meshFormatSize * (i + 2)))));

				// extract the leftmost, upmost, downmost, rightmost grid indices for which the bounding box of the triangle enters
				// TODO: investigate the case where a triangle is added to a grid in which the bounding quad intersects but the triangle does not
				// NOTE: if the grid size is larger than the triangle size, the triangle can be in at most four grid spaces, and the above would only have a small effect
				float offsetX = 0.5f*m_gridSectionsWidth;
				float offsetZ = 0.5f*m_gridSectionsHeight;
				int minGridX = (int)fminf(fminf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
				int minGridZ = (int)fminf(fminf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);
				int maxGridX = (int)fmaxf(fmaxf(p0.GetX() / m_gridScale + offsetX, p1.GetX() / m_gridScale + offsetX), p2.GetX() / m_gridScale + offsetX);
				int maxGridZ = (int)fmaxf(fmaxf(p0.GetZ() / m_gridScale + offsetZ, p1.GetZ() / m_gridScale + offsetZ), p2.GetZ() / m_gridScale + offsetZ);

				// error checking
				if (minGridX < 0 || minGridX > m_gridSectionsWidth || minGridZ < 0 || minGridZ > m_gridSectionsHeight || maxGridX < 0 || maxGridX > m_gridSectionsWidth || maxGridZ < 0 || maxGridZ > m_gridSectionsHeight)
				{
					GameLogger::Log(MessageType::cWarning, "Tried to AddGraphicalObject to SpatialGrid but some triangles were out of grid range!\n");
					return false;
				}

				// add to all grid cells in range
				for (int x = minGridX; x <= maxGridX; ++x)
				{
					for (int z = minGridZ; z <= maxGridZ; ++z)
					{
						SpatialTriangleData newData;
						newData.m_pTriangleOwner = pGraphicalObjectToAdd;
						newData.m_triangleVertexZeroIndex = i; // pIndices[i] is index of p0.
						int w = m_pGridStartIndices[z*m_gridSectionsWidth + x] + m_pGridTriangleCounts[z*m_gridSectionsWidth + x];
						m_pData[w] = newData;
						m_pGridTriangleCounts[z*m_gridSectionsWidth + x]++;
					}
				}
			}
		}

		// indicate success
		GameLogger::Log(MessageType::Process, "Successfully added GraphicalObject to SpatialGrid!\n");
		return true;
	}

	void SpatialGrid::CleanUp()
	{
		if (m_pData) { delete[] m_pData; }
		if (m_pGridStartIndices) { delete[] m_pGridStartIndices; }
		if (m_pGridTriangleCounts) { delete[] m_pGridTriangleCounts; }
	}
}
