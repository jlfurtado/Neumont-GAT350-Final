#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

// Justin Furtado
// 7/31/2016
// VertexFormat.h
// Holds information about the format for vertices

#include "ExportHeader.h"

namespace Engine
{
	enum class ENGINE_SHARED VertexFormat
	{
		None = 0, // used to indicate non-initialized format variables
		HasPosition = 1,
		HasColor = 2,
		HasTexture = 4,
		HasNormal = 8,
		PositionOnly = HasPosition,
		PositionColor = HasPosition | HasColor,
		PositionTexture = HasPosition | HasTexture,
		PositionNormal = HasPosition | HasNormal,
		PositionColorNormal = HasPosition | HasColor | HasNormal,
		PositionTextureNormal = HasPosition | HasTexture | HasNormal,
		PositionColorTexture = HasPosition | HasColor | HasTexture,
		PositionColorTextureNormal = HasPosition | HasColor | HasTexture | HasNormal
	};

	ENGINE_SHARED int operator& (VertexFormat left, VertexFormat right);
	ENGINE_SHARED VertexFormat operator|(VertexFormat left, VertexFormat right);
	const int POSITION_BYTES = 3 * sizeof(float);
	const int COLOR_BYTES = 3 * sizeof(float);
	const int TEXTURE_BYTES = 2 * sizeof(float);
	const int NORMAL_BYTES = 3 * sizeof(float);
	ENGINE_SHARED int VertexFormatSize(VertexFormat format);
	ENGINE_SHARED VertexFormat VertexFormatFromString(const char *const string);
}

#endif // ifndef VERTEXFORMAT_H