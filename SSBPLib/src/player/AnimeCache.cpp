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
	releseReference();
}


/**
* packNameとanimeNameを指定してAnimeRefを得る
*/
AnimeRef* AnimeCache::getReference(const std::string& packName, const std::string& animeName)
{
	std::string key = toPackAnimeKey(packName, animeName);
	AnimeRef* ref = _dic.at(key);
	return ref;
}

/**
* animeNameのみ指定してAnimeRefを得る
*/
AnimeRef* AnimeCache::getReference(const std::string& animeName)
{
	AnimeRef* ref = _dic.at(animeName);
	return ref;
}

void AnimeCache::dump()
{
	std::map<std::string, AnimeRef*>::iterator it = _dic.begin();
	while(it != _dic.end())
	{
		SS_LOG("%s", (*it).second);
		++it;
	}
}


void AnimeCache::init(const ProjectData* data)
{
	SS_ASSERT_LOG(data != NULL, "Invalid data");

	ToPointer ptr(data);
	const AnimePackData* animePacks = ptr.toAnimePackDatas(data);

	for(int packIndex = 0; packIndex < data->numAnimePacks; packIndex++)
	{
		const AnimePackData* pack = &animePacks[packIndex];
		const AnimationData* animations = ptr.toAnimationDatas(pack);
		const char* packName = ptr.toString(pack->name);

		for(int animeIndex = 0; animeIndex < pack->numAnimations; animeIndex++)
		{
			const AnimationData* anime = &animations[animeIndex];
			const char* animeName = ptr.toString(anime->name);

			AnimeRef* ref = new AnimeRef();
			ref->packName = packName;
			ref->animeName = animeName;
			ref->animationData = anime;
			ref->animePackData = pack;

			// packName + animeNameでの登録
			std::string key = toPackAnimeKey(packName, animeName);
			SS_LOG("anime key: %s", key.c_str());
			_dic.insert(std::map<std::string, AnimeRef*>::value_type(key, ref));

			// animeNameのみでの登録
			//				_dic.insert(std::map<std::string, AnimeRef*>::value_type(animeName, ref));

		}
	}
}

std::string AnimeCache::toPackAnimeKey(const std::string& packName, const std::string& animeName)
{
	//return Format("%s/%s", packName.c_str(), animeName.c_str());
	return packName + "/" + animeName;
}

//キャッシュの削除
void AnimeCache::releseReference(void)
{
	std::map<std::string, AnimeRef*>::iterator it = _dic.begin();
	while(it != _dic.end())
	{
		AnimeRef* ref = it->second;
		if(ref)
		{
			delete ref;
			it->second = 0;
		}
		it++;
	}
	_dic.clear();
}


} //namespace ss
