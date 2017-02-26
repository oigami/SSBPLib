#include "ResourceManager.h"
#include <string>
#include "SS5PlayerData.h"
#include "SS5PlayerPlatform.h"
#include "player/Util.h"
#include "player/ToPointer.h"
#include "player/CellCache.h"
#include "player/AnimeCache.h"
#include "player/EffectCache.h"
#include "player/ResourceSet.h"


namespace ss{

const std::string ResourceManager::s_null;



ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	unregistAll();
}



int ResourceManager::regist(const void *data, size_t dataSize, const std::string &dataKey, const std::string &imageBaseDir)
{
	SS_ASSERT_LOG(data, "Invalid data");
	SS_ASSERT_LOG(dataSize > 0, "dataSize is zero");
	//登録済みかどうかの判定
	if(_dataDic.find(dataKey) != _dataDic.end()){
		RefcountResourceSet* ref = _dataDic.at(dataKey);
		ref->incCount();	//登録済みの場合はカウントアップするだけ。dataの内容は無視(最初に登録されてたもの優先)
		return ref->getCount();
	}

	/***** 新規登録 *****/

	//画像ファイルのディレクトリパスを作る
	std::string baseDir = getImageBaseDir(imageBaseDir, static_cast<const ProjectData*>(data));

	//データを作って登録
	RefcountResourceSet* rs = new RefcountResourceSet(static_cast<const char*>(data), dataSize, baseDir);
	
	_dataDic.insert(std::make_pair(dataKey, rs));
	return rs->getCount();
}


void ResourceManager::unregist(const std::string& dataKey)
{
	auto it = _dataDic.find(dataKey);
	SS_ASSERT(it != _dataDic.end());

	RefcountResourceSet* ref = it->second;
	ref->decCount();
	SS_ASSERT(ref->getCount() >= 0);	//マイナスにはならない

	if(ref->getCount() == 0){
		//消してOKなので消す	
		SS_SAFE_DELETE(ref);
		_dataDic.erase(it);
	}
}

void ResourceManager::unregistAll()
{
	for(auto &str_rs : _dataDic){
		SS_SAFE_DELETE(str_rs.second);
	}
	_dataDic.clear();
}



void ResourceManager::getTextureList(std::vector<std::string> *textureList, const std::string &dataKey) const
{
	const ResourceSet *rs = getData(dataKey);
	const CellCache *cellCache = rs->m_cellCache.get();

	//todo:ss5playerにも似たコードがある・・・resourcesetあたりに一覧取得機能持たせた方がいいかもしれない
	int cellMapNum = cellCache->getCellMapNum();
	textureList->resize(cellMapNum);
	for (int i = 0; i < cellMapNum; ++i){
		(*textureList)[i] = cellCache->getTexturePath(i);
	}
}

Player* ResourceManager::createPlayer(const std::string& dataKey,/* SS5Renderer *renderer,*/ SS5EventListener* eventListener) const
{
	const ResourceSet* rs = getData(dataKey);
	return new Player(rs, /*renderer,*/ eventListener);
}

void ResourceManager::destroyPlayer(Player *&player) const
{
	delete player;
	player = nullptr;
}




std::string ResourceManager::getImageBaseDir(const std::string &imageBaseDir, const ProjectData *data) const
{
	if(imageBaseDir == s_null){	// imageBaseDirの指定がないときはパスを作る

		if(data->imageBaseDir){
			// コンバート時に指定されたパスを使用する
			ToPointer ptr(data);
			return ptr.toString(data->imageBaseDir);
		}
	}
	return imageBaseDir;
}

const ResourceSet* ResourceManager::getData(const std::string& dataKey) const
{
	auto it = _dataDic.find(dataKey);
	SS_ASSERT(it != _dataDic.end());

	RefcountResourceSet* rrs = it->second;
	return rrs->getResourceSet();
}

} //namespace ss
