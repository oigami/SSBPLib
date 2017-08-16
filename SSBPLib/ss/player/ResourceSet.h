#pragma once
#include <memory>
#include <vector>
#include "SS5PlayerData.h"
#include "EffectCache.h"
#include "CellCache.h"
#include "AnimeCache.h"
#include "Util.h"

namespace ss{

/**
 * ResourceSet
 */
class ResourceSet{
private:
	static const ss_u32 DATA_ID = 0x42505353;	//データのチェック値
	static const ss_u32 DATA_VERSION = 4;		//データのチェック値

	std::vector<char> m_src;						//データの実体

	bool m_hasError = false;
public:
	const ProjectData* m_data;						//データを指すだけ
	std::unique_ptr<EffectCache> m_effectCache;
	std::unique_ptr<CellCache> m_cellCache;			//cell周りの構造へのアクセサ
	std::unique_ptr<AnimeCache> m_animeCache;		//anim周りの構造へのアクセサ

	/** dataはコピーされます */
	ResourceSet(const char *data, size_t dataSize, const std::string &imageBaseDir)
		: m_src(data, data + dataSize)	//コピー
		, m_data(nullptr)
		, m_effectCache(nullptr)
		, m_cellCache(nullptr)
		, m_animeCache(nullptr)
	{
		SS_ASSERT_LOG(data, "Invalid data");
		SS_ASSERT_LOG(dataSize > 0, "dataSize is zero");

		m_data = reinterpret_cast<const ProjectData*>(&m_src[0]);
		if (m_data->dataId != DATA_ID) {
			m_hasError = true;
			return;
		}

		if(m_data->version != DATA_VERSION) {
			m_hasError = true;
			return;
		}

		//エフェクトはセルを参照するのでこの順番で生成する必要がある
		m_cellCache.reset(new CellCache(m_data, imageBaseDir));
		m_effectCache.reset(new EffectCache(m_data, m_cellCache.get()));
		m_animeCache.reset(new AnimeCache(m_data));
	}

	~ResourceSet(){
	}

	explicit operator bool() const { return !isEmpty(); }

	bool isEmpty() const { return m_hasError; }
};

} //namespace ss
