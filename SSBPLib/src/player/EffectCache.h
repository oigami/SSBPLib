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
	EffectCache();
	~EffectCache();

	static EffectCache* create(const ProjectData* data, const std::string& imageBaseDir, CellCache* cellCache);
	
	/**
	 * エフェクトファイル名を指定してEffectRefを得る
	 */
	SsEffectModel* getReference(const std::string& name);

	void dump();

private:
	void init(const ProjectData* data, const std::string& imageBaseDir, CellCache* cellCache);

	//エフェクトファイル情報の削除
	void releseReference(void);

	
	std::map<std::string, SsEffectModel*>		_dic;
};

} //namespace ss
