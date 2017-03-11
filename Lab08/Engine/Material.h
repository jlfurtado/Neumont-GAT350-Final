#ifndef MATERIAL_H
#define MATERIAL_H

// Justin Furtado
// 1/13/2017
// Material.h
// Stores data for lighting a GOBJECT

#include "Vec3.h"
#include "ExportHeader.h"

namespace Engine
{
	struct ENGINE_SHARED Material
	{
	public:
		Vec3 m_ambientReflectivity{ 1.0f, 1.0f, 1.0f };
		Vec3 m_diffuseReflectivity{ 1.0f, 1.0f, 1.0f };
		Vec3 m_specularReflectivity{ 1.0f, 1.0f, 1.0f };
		Vec3 m_materialColor{ 1.0f, 1.0f, 1.0f };
		float m_specularIntensity{ 1.0f };
	};
}


#endif // ifndef MATERIAL_H