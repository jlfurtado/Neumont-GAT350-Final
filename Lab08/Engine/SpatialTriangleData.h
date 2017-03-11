#ifndef SPATIALTRIANGLEDATA_H
#define SPATIALTRIANGLEDATA_H

// Justin Furtado
// 8/24/2016
// SpatialTriangleData.h
// A simple struct used to store data about triangles in a grid cell for spatial partitioning

namespace Engine
{
	class GraphicalObject;

	struct SpatialTriangleData
	{
	public:
		SpatialTriangleData()
			: m_pTriangleOwner(nullptr), m_triangleVertexZeroIndex(-1) {}

		GraphicalObject *m_pTriangleOwner;
		int m_triangleVertexZeroIndex;
	};
}

#endif // ifndef SPATIALTRIANGLEDATA_H