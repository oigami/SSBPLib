#pragma once
#include "Util.h"

namespace ss{

/**
 * ResourceSet
 */
struct ResourceSet
{
	const ProjectData* m_data;
	EffectCache* m_effectCache;
	CellCache* m_cellCache;
	AnimeCache* m_animeCache;

	~ResourceSet(){
		SS_SAFE_DELETE(m_data);
		SS_SAFE_DELETE(m_animeCache);
		SS_SAFE_DELETE(m_cellCache);
		SS_SAFE_DELETE(m_effectCache);
	}
};

} //namespace ss
