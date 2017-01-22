#pragma once
#include <vector>
#include <string>
#include "SS5PlayerTypes.h"
#include "common/Loader/sstypes.h"
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
	int m_cellMapIndex;			//セルマップ番号
	SSRect		m_rect;			//cellの範囲
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
	int indexOfCell(const std::string &cellName) const;

	//指定したCellMapのテクスチャ名を取得する
	std::string getTexturePath(int cellMapIndex) const;

	//CellMap数
	int getCellMapNum() const;

	
private:
	/** dataを元にCellRefを構築する*/
	void init(const ProjectData* data, const std::string& imageBaseDir);

	std::string m_imageBaseDir;
	std::vector<const char *> m_imagePaths;	//テクスチャファイル名(添字はセルマップ番号に対応)
	std::vector<CellRef> m_cellRefs;		//数が動的に変化することはないので実体を入れる事にした
};

} //namespace ss
