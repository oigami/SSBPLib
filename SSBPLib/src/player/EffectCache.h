#pragma once
#include <string>
#include <map>

namespace ss{
struct ProjectData;
class CellCache;
class SsEffectModel;

/**
 * EffectCache
 */
class EffectCache{
public:
	EffectCache(const ProjectData* data, const CellCache* cellCache);
	~EffectCache();
	
	/**
	 * エフェクトファイル名を指定してEffectRefを得る
	 */
	const SsEffectModel* getReference(const std::string& name) const;

private:
	void init(const ProjectData* data, const CellCache* cellCache);

	
	std::map<std::string, const SsEffectModel*> m_dic;
};

} //namespace ss
