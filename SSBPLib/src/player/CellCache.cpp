#include "CellCache.h"
#include <vector>
#include <string>
#include "SS5PlayerPlatform.h"
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
	releseReference();
}


const CellRef* CellCache::getReference(int index) const
{
	SS_ASSERT_LOG(index >= 0 && index < m_cellRefs.size(), "index is out of range > %d", index);
	return &(m_cellRefs[index]);
}

int CellCache::indexOfCell(const std::string &cellName) const
{
	//cellnameは同名も存在できるようだが、ひとまず最初に見つかったものを返すことにする
	for(int i = 0; i < m_cellRefs.size(); ++i){
		const CellRef *ref = getReference(i);
		if(cellName == ref->m_cellName){
			return i;		//名前一致したので返す
		}
	}
	return -1;				//名前一致しなかったとき
}



//データを見てcellrefとimagepathを構築
void CellCache::init(const ProjectData* data, const std::string& imageBaseDir)
{
	SS_ASSERT_LOG(data != NULL, "Invalid data");
	
	m_imageBaseDir = imageBaseDir;
	m_cellRefs.resize(data->numCells);	//cell数だけ領域確保しておく
	std::map<int, const char*> imagePathMap;	//数がわからないのでひとまず<index,path>のmapにしておく

	
	ToPointer ptr(data);
	const Cell* cells = ptr.toCells(data);

	for(int i = 0; i < data->numCells; i++){
		const Cell* cell = &cells[i];
		const CellMap* cellMap = ptr.toCellMap(cell);

		const char* cellname = ptr.toString(cell->name);			//セル名
		//const char* cellmapname = ptr.toString(cellMap->name);	//セルマップ名

		//ここではロードなどはせずに番号を保存しとくだけに留める
		imagePathMap[cellMap->index] = ptr.toString(cellMap->imagePath);		//memo:ここは何度も上書きされるだろうがconst char*のコピーなので大丈夫

	#if 0
		//todo: これを外側のクラスに任せるようにすること!
		if(cellMap->index >= (int)m_textures.size())
		{
			const char* imagePath = ptr.toString(cellMap->imagePath);
			addTexture(imagePath, imageBaseDir, (SsTexWrapMode::_enum)cellMap->wrapmode, (SsTexFilterMode::_enum)cellMap->filtermode);
		}
	#endif

		CellRef ref = {
			cell,  cellname, cellMap->index, //m_textures[cellMap->index],
			SSRect(cell->x, cell->y, cell->width, cell->height)
		};
		m_cellRefs[i] = ref;
	}

	//map --> vector に詰め直す -----------------
	auto it = std::max_element(imagePathMap.begin(), imagePathMap.end());
	if(it == imagePathMap.end()){			//画像無し
		return;
	}

	int maxIndex = it->first;
	m_imagePaths.resize(maxIndex + 1);
	for(auto &index_path : imagePathMap){	//詰め替え
		m_imagePaths[index_path.first] = index_path.second;
	}
}


//キャッシュの削除
void CellCache::releseReference(void)
{
#if 0
	for(TextuerData& tex : m_textures){
		SSTextureRelese(tex.handle);
	}
#endif
}

void CellCache::addTexture(const std::string& imagePath, const std::string& imageBaseDir, SsTexWrapMode::_enum  wrapmode, SsTexFilterMode::_enum filtermode)
{
#if 0
	std::string path = "";

	if(isAbsolutePath(imagePath))
	{
		// 絶対パスのときはそのまま扱う
		path = imagePath;
	}
	else
	{
		// 相対パスのときはimageBaseDirを付与する
		path.append(imageBaseDir);
		size_t pathLen = path.length();
		if(pathLen && path.at(pathLen - 1) != '/' && path.at(pathLen - 1) != '\\')
		{
			path.append("/");
		}
		path.append(imagePath);
	}

	//テクスチャの読み込み
	long tex = SSTextureLoad(path.c_str(), wrapmode, filtermode);
	SS_LOG("load: %s", path.c_str());
	TextuerData texdata;
	texdata.handle = tex;
	int w;
	int h;
	SSGetTextureSize(texdata.handle, w, h);
	texdata.size_w = w;
	texdata.size_h = h;

	m_textures.push_back(texdata);
#endif
}


std::string CellCache::getTexturePath(int cellMapIndex) const
{
	SS_ASSERT(cellMapIndex >= 0 && cellMapIndex < m_imagePaths.size());	//indexのassertチェックはする
#if 0
	if(isAbsolutePath(m_imagePaths[cellMapIndex])){
		return m_imagePaths[cellMapIndex];	//絶対パスのときはそのまま扱う
	}
#endif
	std::string texturePath = m_imageBaseDir + m_imagePaths[cellMapIndex];
	return texturePath;
}


int CellCache::getCellMapNum() const{
	return m_imagePaths.size();
}



} //namespace ss
