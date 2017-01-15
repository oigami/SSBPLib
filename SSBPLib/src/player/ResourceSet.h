#pragma once
#include <memory>
#include "SS5PlayerData.h"
#include "EffectCache.h"
#include "CellCache.h"
#include "AnimeCache.h"
#include "Util.h"

namespace ss{

/**
 * ResourceSet
 */
struct ResourceSet
{
	const ProjectData* m_data;
	std::unique_ptr<EffectCache> m_effectCache;
	std::unique_ptr<CellCache> m_cellCache;
	std::unique_ptr<AnimeCache> m_animeCache;

	ResourceSet(const ProjectData* data, const std::string &imageBaseDir)
		: m_data(data)
		, m_effectCache(nullptr)
		, m_cellCache(nullptr)
		, m_animeCache(nullptr)
	{
		//エフェクトはセルを参照するのでこの順番で生成する必要がある
		m_cellCache.reset(new CellCache(data, imageBaseDir));
		m_effectCache.reset(new EffectCache(data, imageBaseDir, m_cellCache.get()));
		m_animeCache.reset(new AnimeCache(data));
	}

	~ResourceSet(){
		SS_SAFE_DELETE(m_data);
	}
};

} //namespace ss
