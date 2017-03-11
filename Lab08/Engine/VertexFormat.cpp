#include "VertexFormat.h"
#include "StringFuncs.h"

// Justin Furtado
// 7/31/2016
// VertexFormat.cpp
// Holds information about the format for vertices

namespace Engine
{
	int operator&(VertexFormat left, VertexFormat right)
	{
		return static_cast<int>(left) & static_cast<int>(right);
	}

	VertexFormat operator|(VertexFormat left, VertexFormat right)
	{
		return static_cast<VertexFormat>(static_cast<int>(left) | static_cast<int>(right));
	}

	int VertexFormatSize(VertexFormat format)
	{
		return  ((format & VertexFormat::HasPosition) ? POSITION_BYTES : 0) +
			((format & VertexFormat::HasColor) ? COLOR_BYTES : 0) +
			((format & VertexFormat::HasTexture) ? TEXTURE_BYTES : 0) +
			((format & VertexFormat::HasNormal) ? NORMAL_BYTES : 0);
	}

	VertexFormat VertexFormatFromString(const char * const string)
	{
		// All vertex formats have position, for now
		if (!(string[0] == 'p' || string[0] == 'P')) { return VertexFormat::None; }

		// if no character to indicate color, or with 0 to have no effect, else add color, repeat for all formats 
		return  (VertexFormat::HasPosition) |
			(((StringFuncs::FindSubString(string, "c") < 0) && (StringFuncs::FindSubString(string, "C") < 0)) ? VertexFormat::None : VertexFormat::HasColor) |
			(((StringFuncs::FindSubString(string, "t") < 0) && (StringFuncs::FindSubString(string, "T") < 0)) ? VertexFormat::None : VertexFormat::HasTexture) |
			(((StringFuncs::FindSubString(string, "n") < 0) && (StringFuncs::FindSubString(string, "N") < 0)) ? VertexFormat::None : VertexFormat::HasNormal);
	}
}
