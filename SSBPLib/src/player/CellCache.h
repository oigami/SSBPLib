#pragma once
#include <vector>
#include <string>
#include "SS5PlayerTypes.h"
#include "effect/sstypes.h"
#include "math/SSRect.h"

namespace ss{
struct ProjectData;
struct Cell;

/**
 * CellRef
 */
struct CellRef{
	const Cell* m_cell;
	const char* m_cellName;		//cellの名前
	int			m_cellMapIndex;	//セルマップ番号
	const char* m_cellMapName;	//セルマップ名
	SSRect		m_rect;			//cellの範囲
};

/**
 * CellMapに近いがテクスチャの設定のみなので別名にします
 */
struct CellMapTextureInfo{
	const char* m_imagePaths;				//セルマップ名
	SsTexWrapMode m_wrapMode;		//ラップモード
	SsTexFilterMode m_filterMode;	//フィルタモード
	
/*	CellMapTextureInfo()
		: m_imagePaths(nullptr)
		, m_wrapMode(SsTexWrapMode::invalid)
		, m_filterMode(SsTexFilterMode::invalid){} */

	bool operator <(const CellMapTextureInfo& o) const{
		return m_imagePaths < o.m_imagePaths;	//mapに突っ込むのにひとまず定義しておく
	}
};


/**
 * CellCache
 *  生のデータを扱いやすくするため、Cell周りに関しての構築を行う
 *  dataを元にして、CellRefと画像パスを構築する
 */
class CellCache{
public:
	/** dataを元にCellRefを構築する */
	CellCache(const ProjectData* data, const std::string& imageBaseDir);
	~CellCache();

	/** 指定番号のCellRefを返す */
	const CellRef* getReference(int index) const;
	
	/** cell名からindexを返す */
	int indexOfCell(const std::string& cellName, const std::string& cellMapName) const;

	//指定したCellMapのテクスチャ名を取得する
	std::string getTexturePath(int cellMapIndex) const;
	SsTexWrapMode getWrapMode(int cellMapIndex) const;
	SsTexFilterMode getFilterMode(int cellMapIndex) const;

	//CellMap数
	int getCellMapNum() const;

	
private:
	/** dataを元にCellRefを構築する*/
	void init(const ProjectData* data, const std::string& imageBaseDir);

	std::string m_imageBaseDir;
	std::vector<CellMapTextureInfo> m_textureInfos;	//テクスチャファイル情報(添字はセルマップ番号に対応)
	std::vector<CellRef> m_cellRefs;				//数が動的に変化することはないので実体を入れる事にした
};

} //namespace ss
