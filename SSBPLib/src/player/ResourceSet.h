#pragma once
#include "Util.h"

namespace ss{

/**
 * ResourceSet
 */
struct ResourceSet
{
	const ProjectData* data;
	EffectCache* effectCache;
	CellCache* cellCache;
	AnimeCache* animeCache;

	~ResourceSet(){
		SS_SAFE_DELETE(data);
		SS_SAFE_DELETE(animeCache);
		SS_SAFE_DELETE(cellCache);
		SS_SAFE_DELETE(effectCache);
	}
};

} //namespace ss
