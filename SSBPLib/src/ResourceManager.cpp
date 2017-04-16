#include "ResourceManager.h"
#include <string>
#include "SS5Player.h"
#include "SS5Effect.h"
#include "SS5PlayerData.h"
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



int ResourceManager::regist(
	const void* data,
	size_t dataSize,
	const std::string& dataKey,
	const std::string& imageBaseDir,
	PreloadCallback texturePreloadCallbackFunc
){
	SS_ASSERT_LOG(data, "Invalid data");
	SS_ASSERT_LOG(dataSize > 0, "dataSize is zero");
	//登録済みかどうかの判定
	if(_dataDic.find(dataKey) != _dataDic.end()){
		RefcountResourceSet* ref = _dataDic.at(dataKey);
		ref->incCount();	//登録済みの場合はカウントアップするだけ。dataの内容は無視(最初に登録されてたもの優先)

		texturePreload(ref->getResourceSet(), texturePreloadCallbackFunc);	//テクスチャ事前読み込みのコールバック
		return ref->getCount();
	}

	/***** 新規登録 *****/

	//画像ファイルのディレクトリパスを作る
	std::string baseDir = getImageBaseDir(imageBaseDir, static_cast<const ProjectData*>(data));

	//データを作って登録
	RefcountResourceSet* rs = new RefcountResourceSet(static_cast<const char*>(data), dataSize, baseDir);
	_dataDic.insert(std::make_pair(dataKey, rs));

	texturePreload(rs->getResourceSet(), texturePreloadCallbackFunc);	//テクスチャ事前読み込みのコールバック
	return rs->getCount();
}


int ResourceManager::unregist(const std::string& dataKey)
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
		return 0;				//deleteしたので参照カウンタは0
	}
	
	return ref->getCount();		//参照カウンタが残っているとき
}

void ResourceManager::unregistAll()
{
	for(auto& str_rs : _dataDic){
		SS_SAFE_DELETE(str_rs.second);
	}
	_dataDic.clear();
}



void ResourceManager::getTextureList(std::vector<std::string>* textureList, const std::string& dataKey) const
{
	const ResourceSet* rs = getData(dataKey);
	const CellCache* cellCache = rs->m_cellCache.get();

	//todo:ss5playerにも似たコードがある・・・resourcesetあたりに一覧取得機能持たせた方がいいかもしれない
	int cellMapNum = cellCache->getCellMapNum();
	textureList->resize(cellMapNum);
	for (int i = 0; i < cellMapNum; ++i){
		(*textureList)[i] = cellCache->getTexturePath(i);
	}
}

//player
Player* ResourceManager::createPlayer(SS5EventListener* eventListener, const std::string& dataKey, const std::string& animeName) const
{
	const ResourceSet* rs = getData(dataKey);
	//アニメーションの指定が無い場合は、最初のものを入れておく
	if(animeName == s_null){
		return new Player(eventListener, rs, rs->m_animeCache->getFirstAnimationName());
	}
	return new Player(eventListener, rs, animeName);
}
void ResourceManager::destroyPlayer(Player*& player) const
{
	delete player;
	player = nullptr;
}

//effect
SS5Effect* ResourceManager::createEffect(SS5EventListener* eventListener, const std::string& dataKey, const std::string& effectName, int seed) const
{
	const ResourceSet* rs = getData(dataKey);
	return new SS5Effect(eventListener, rs, effectName, seed);
}
void ResourceManager::destroyEffect(SS5Effect*& effect) const
{
	delete effect;
	effect = nullptr;
}



std::string ResourceManager::getImageBaseDir(const std::string& imageBaseDir, const ProjectData* data) const
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


//事前読み込みさせる
void ResourceManager::texturePreload(const ResourceSet* resource, PreloadCallback texturePreloadCallbackFunc) const
{
	int cellMapNum = resource->m_cellCache->getCellMapNum();
	for(int i = 0; i < cellMapNum; ++i){
		const std::string& textureName = resource->m_cellCache->getTexturePath(i);
		SsTexWrapMode wrapmode = resource->m_cellCache->getWrapMode(i);
		SsTexFilterMode filtermode = resource->m_cellCache->getFilterMode(i);

		texturePreloadCallbackFunc(textureName, wrapmode, filtermode);
	}
}


} //namespace ss
