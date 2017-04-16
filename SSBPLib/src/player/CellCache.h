#pragma once
#include <vector>
#include <string>
#include "SS5PlayerTypes.h"
#include "PlayerDef.h"
#include "math/SSRect.h"

namespace ss{
struct ProjectData;
struct Cell;

/**
 * CellRef
 */
struct CellRef{
	//const Cell* m_cell;
	std::string m_cellPath;		//cellの名前("セルマップ名/セル名"の形式)
	int			m_cellMapIndex;	//セルマップ番号
	SSRect		m_rect;			//cellの範囲
	Vector2		m_pivot;		//原点補正
	Vector2		m_uv1;			//uv1
	Vector2		m_uv2;			//uv2
};

/**
 * CellMapに近いがテクスチャの設定のみなので別名にします
 */
struct CellMapTextureInfo{
	std::string m_imagePaths;		//画像ファイルパス("imageBaseDir/texturename"の形式で保存されます)
	SsTexWrapMode m_wrapMode;		//ラップモード
	SsTexFilterMode m_filterMode;	//フィルタモード
#if 0	
	CellMapTextureInfo(const std::string& imageBaseDir, const std::string& textureName, SsTexWrapMode wrapMode, SsTexFilterMode filterMode)
		: m_imagePaths(imageBaseDir + textureName)
		, m_wrapMode(wrapMode)
		, m_filterMode(filterMode){}
#endif
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
	
	/** cell名からindexを返す。cell名は"セルマップ名/セル名"の形式 */
	int indexOfCell(const std::string& cellName) const;

	//指定したCellMapのテクスチャ名を取得する
	std::string getTexturePath(int cellMapIndex) const;
	SsTexWrapMode getWrapMode(int cellMapIndex) const;
	SsTexFilterMode getFilterMode(int cellMapIndex) const;

	//CellMap数
	int getCellMapNum() const;

	
private:
	/** dataを元にCellRefを構築する*/
	void init(const ProjectData* data, const std::string& imageBaseDir);

	std::vector<CellMapTextureInfo> m_textureInfos;	//テクスチャファイル情報(添字はセルマップ番号に対応)
	std::vector<CellRef> m_cellRefs;				//数が動的に変化することはないので実体を入れる事にした
};

} //namespace ss
