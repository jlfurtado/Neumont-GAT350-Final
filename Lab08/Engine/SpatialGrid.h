#ifndef SPATIALGRID_H
#define SPATIALGRID_H

// Justin Furtado
// SpatialGrid.h
// 8/24/2016
// Holds an array of linked lists of SpatialTriangleData used for spatial partitioning

#include "SpatialTriangleData.h"
#include "ExportHeader.h"

namespace Engine
{
	class ENGINE_SHARED SpatialGrid
	{
	public:
		SpatialGrid();
		~SpatialGrid();

		SpatialTriangleData *GetTriangleDataByGrid(int gridX, int gridZ);
		SpatialTriangleData *GetTriangleDataByGridAtPosition(float worldX, float worldZ);
		bool AddGraphicalObject(GraphicalObject *pGraphicalObjectToAdd);
		float GetGridScale();
		int GetGridTriangleCount(int gridX, int gridZ);
		void CalculateStatisticsFromCounts();
		void RemoveGraphicalObject(GraphicalObject *pGobToRemove);
		int GetGridWidth();
		int GetGridHeight();
		void ConsoleLogStats();
		bool AddTrianglesToPartitions();

		// TODO: Move!??!?!?!

	private:
		bool AddGraphicalObjectToGrid(GraphicalObject *pGraphicalObjectToAdd);
		bool SetGridStartIndices();
		void CleanUp();

		float m_gridScale;
		SpatialTriangleData *m_pData{ nullptr };
		GraphicalObject *m_pHeadNode{ nullptr };
		int *m_pGridStartIndices{ nullptr };
		int *m_pGridTriangleCounts{ nullptr };
		int m_gridSectionsWidth{ 85 };
		int m_gridSectionsHeight{ 85 };
		int m_totalGridSections{ m_gridSectionsHeight*m_gridSectionsWidth };
		int m_graphicalObjectCount{ 0 };
		int m_minGridTriangleCount{ -1 };
		int m_maxGridTriangleCount{ -1 };
		int m_totalTriangleCount{ -1 };
		float m_avgGridTriangleCount{ -1.0f };
	};
}

#endif // ifndef SPATIALGRID_H