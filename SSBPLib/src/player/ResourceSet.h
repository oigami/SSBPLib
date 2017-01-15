#pragma once


namespace ss{

/**
 * ResourceSet
 */
struct ResourceSet
{
	const ProjectData* data;
	bool isDataAutoRelease;
	EffectCache* effectCache;
	CellCache* cellCache;
	AnimeCache* animeCache;

	virtual ~ResourceSet()
	{
		if(isDataAutoRelease)
		{
			delete data;
			data = NULL;
		}
		if(animeCache)
		{
			delete animeCache;
			animeCache = NULL;
		}
		if(cellCache)
		{
			delete cellCache;
			cellCache = NULL;
		}
		if(effectCache)
		{
			delete effectCache;
			effectCache = NULL;
		}
	}
};

} //namespace ss
