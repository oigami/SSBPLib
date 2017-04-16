#include "CellCache.h"
#include <vector>
#include <string>
#include <map>
#include "SS5PlayerTypes.h"
#include "SS5PlayerData.h"
#include "player/Util.h"
#include "player/ToPointer.h"


namespace ss{


CellCache::CellCache(const ProjectData* data, const std::string& imageBaseDir)
{
	init(data, imageBaseDir);
}
CellCache::~CellCache()
{
}


const CellRef* CellCache::getReference(int index) const
{
	SS_ASSERT_LOG(index >= 0 && index < m_cellRefs.size(), "index is out of range > %d", index);
	return &(m_cellRefs[index]);
}

int CellCache::indexOfCell(const std::string& cellName) const
{
	//cellnameは同名も存在できるようだが、ひとまず最初に見つかったものを返すことにする
	for(int i = 0; i < m_cellRefs.size(); ++i){
		const CellRef* ref = getReference(i);
		if(cellName == ref->m_cellPath){
			return i;		//名前一致したので返す
		}
	}
	return -1;				//名前一致しなかったとき
}



//データを見てcellrefとimagepathを構築
void CellCache::init(const ProjectData* data, const std::string& imageBaseDir)
{
	SS_ASSERT_LOG(data != nullptr, "Invalid data");
	
	m_cellRefs.resize(data->numCells);	//cell数だけ領域確保しておく
	std::map<int, CellMapTextureInfo> textureInfoMap;	//数がわからないのでひとまず<index,info>のmapにしておく

	
	ToPointer ptr(data);
	const Cell* cells = ptr.toCells(data);

	for(int i = 0; i < data->numCells; i++){
		const Cell* cell = &cells[i];
		const CellMap* cellMap = ptr.toCellMap(cell);

		//ここではロードなどはせずに番号を保存しとくだけに留める
		//セルは同じセルマップを参照するため、既に入れたかどうかをチェックする
		if(textureInfoMap.find(cellMap->index) == textureInfoMap.end()){
			CellMapTextureInfo info = {
				imageBaseDir + ptr.toString(cellMap->imagePath),
				static_cast<SsTexWrapMode>(cellMap->wrapmode),
				static_cast<SsTexFilterMode>(cellMap->filtermode)
			};
			textureInfoMap.insert(std::make_pair(cellMap->index, info));
		}

		const char* cellname = ptr.toString(cell->name);			//セル名
		std::string cellmapname = ptr.toString(cellMap->name);		//セルマップ名
		CellRef ref = {	/*cell,*/
			cellmapname + "/" + cellname,
			cellMap->index,
			SSRect(cell->x, cell->y, cell->width, cell->height),
			Vector2(cell->pivot_X, cell->pivot_Y),
			Vector2(cell->u1, cell->v1),
			Vector2(cell->u2, cell->v2)
		};
		m_cellRefs[i] = ref;
	}

	//map --> vector に詰め直す -----------------
	auto it = std::max_element(textureInfoMap.begin(), textureInfoMap.end());
	if(it == textureInfoMap.end()){			//画像無し
		return;
	}

	int maxIndex = it->first;
	m_textureInfos.resize(maxIndex + 1);
	for(auto &index_path : textureInfoMap){	//詰め替え
		m_textureInfos[index_path.first] = index_path.second;
	}
}


const std::string& CellCache::getTexturePath(int cellMapIndex) const
{
	SS_ASSERT(cellMapIndex >= 0 && cellMapIndex < m_textureInfos.size());	//indexのassertチェックはする
#if 0
	if(isAbsolutePath(m_imagePaths[cellMapIndex])){
		return m_imagePaths[cellMapIndex];	//絶対パスのときはそのまま扱う
	}
#endif
	return m_textureInfos[cellMapIndex].m_imagePaths;
}

SsTexWrapMode CellCache::getWrapMode(int cellMapIndex) const{
	return m_textureInfos.at(cellMapIndex).m_wrapMode;
}
SsTexFilterMode CellCache::getFilterMode(int cellMapIndex) const{
	return m_textureInfos.at(cellMapIndex).m_filterMode;
}

int CellCache::getCellMapNum() const{
	return m_textureInfos.size();
}



} //namespace ss
