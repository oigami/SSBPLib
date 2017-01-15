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

static ResourceManager* defaultInstance = NULL;
const std::string ResourceManager::s_null;

ResourceManager* ResourceManager::getInstance()
{
	if(!defaultInstance)
	{
		defaultInstance = ResourceManager::create();
	}
	return defaultInstance;
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	unregistAll();
}

ResourceManager* ResourceManager::create()
{
	ResourceManager* obj = new ResourceManager();
	return obj;
}

ResourceSet* ResourceManager::getData(const std::string& dataKey)
{
	auto it = _dataDic.find(dataKey);
	SS_ASSERT(it != _dataDic.end());

	RefcountResourceSet* rrs = it->second;
	return rrs->getResourceSet();
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

//データ名、セル名を指定して、セルで使用しているテクスチャを変更する
bool ResourceManager::changeTexture(char* ssbpName, char* ssceName, long texture)
{
	bool rc = false;

	ResourceSet* rs = getData(ssbpName);
	rc = rs->m_cellCache->setCellRefTexture(rs->m_data, ssceName, texture);

	return(rc);
}

//指定したデータのテクスチャを破棄します
bool ResourceManager::releseTexture(char* ssbpName)
{

	ResourceSet* rs = getData(ssbpName);
	bool rc = rs->m_cellCache->releseTexture(rs->m_data);

	return(rc);
}

//アニメーションの総フレーム数を取得する
int ResourceManager::getMaxFrame(std::string ssbpName, std::string animeName)
{
	int rc = -1;

	ResourceSet* rs = getData(ssbpName);
	AnimeRef* animeRef = rs->m_animeCache->getReference(animeName);
	if(animeRef == NULL)
	{
		//std::string msg = Format("Not found animation > anime=%s", animeName.c_str());
		std::string msg = std::string("Not found animation > anime=") + animeName;
		SS_ASSERT_LOG(animeRef != NULL, msg.c_str());
	}
	rc = animeRef->animationData->numFrames;

	return(rc);
}

} //namespace ss
