#include "AnimeCache.h"
#include <string>
#include "SS5PlayerData.h"
#include "player/Util.h"
#include "player/ToPointer.h"

namespace ss{


AnimeCache::AnimeCache(const ProjectData* data)
{
	init(data);
}
AnimeCache::~AnimeCache()
{
}


//packNameとanimeNameを指定してAnimeRefを得る
AnimeRef* AnimeCache::getReference(const std::string& packName, const std::string& animeName)
{
	std::string key = toPackAnimeKey(packName, animeName);	//todo:animeNameだけに統一したい
	SS_ASSERT_LOG(m_animeRefs.find(key) != m_animeRefs.end(), "Not found animation");

	return &(m_animeRefs.at(key));
}

//animeNameのみ指定してAnimeRefを得る
AnimeRef* AnimeCache::getReference(const std::string& animeName)
{
	SS_ASSERT_LOG(m_animeRefs.find(animeName) != m_animeRefs.end(), "Not found animation");
	return &(m_animeRefs.at(animeName));
}

void AnimeCache::dump() const
{
	for(auto &str_aref : m_animeRefs){
		SS_LOG("%s", str_aref.first);
	}
}


void AnimeCache::init(const ProjectData* data)
{
	SS_ASSERT_LOG(data, "Invalid data");

	ToPointer ptr(data);
	const AnimePackData* animePacks = ptr.toAnimePackDatas(data);

	for(int packIndex = 0; packIndex < data->numAnimePacks; packIndex++){
		const AnimePackData* pack = &animePacks[packIndex];
		addAnimationData(ptr, pack);		//ssaeからAnimationDataを登録する
	}
}

//ssaeからAnimationDataを登録する
void AnimeCache::addAnimationData(ToPointer ptr, const AnimePackData* pack)
{
	const char* packName = ptr.toString(pack->name);

	const PartData* partDatas = ptr.toPartDatas(pack);				//array
	const AnimationData* animations = ptr.toAnimationDatas(pack);	//array

	for(int animeIndex = 0; animeIndex < pack->numAnimations; animeIndex++){
		const AnimationData* anime = &animations[animeIndex];
		const char* animeName = ptr.toString(anime->name);

		AnimeRef ref = {
			packName, animeName, anime, partDatas, pack->numParts
		};

		// packName + animeNameでの登録
		std::string key = toPackAnimeKey(packName, animeName);
		SS_LOG("anime key: %s", key.c_str());
		m_animeRefs.insert(std::make_pair(key, ref));

		// animeNameのみでの登録
		m_animeRefs.insert(std::make_pair(animeName, ref));
	}
	
}

std::string AnimeCache::toPackAnimeKey(const std::string& packName, const std::string& animeName)
{
	return packName + "/" + animeName;		//return Format("%s/%s", packName.c_str(), animeName.c_str());
}



} //namespace ss
