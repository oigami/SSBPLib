#include "AnimeCache.h"
#include <string>
#include <vector>
#include "SS5PlayerData.h"
#include "player/Util.h"
#include "player/ToPointer.h"
using namespace std;

namespace ss{


AnimeCache::AnimeCache(const ProjectData* data)
{
	init(data);
}
AnimeCache::~AnimeCache()
{
}


//"packname/animename"で指定してAnimeRefを得る
const AnimeRef* AnimeCache::getReference(const string& animeName) const
{
	SS_ASSERT_LOG(m_animeRefs.find(animeName) != m_animeRefs.end(), "Not found animation");
	return &(m_animeRefs.at(animeName));
}


//最初のアニメーション名を得る
const string& AnimeCache::getFirstAnimationName() const
{
	SS_ASSERT_LOG(m_animeRefs.begin() != m_animeRefs.end(), "No Animation");

	auto it = m_animeRefs.begin();
	return it->first;
}

//アニメーションのリストを作る
vector<string> AnimeCache::getAnimationList() const
{
	vector<string> animlist;
	animlist.reserve( m_animeRefs.size() );
	
	for(auto &key_ref : m_animeRefs){
		animlist.push_back(key_ref.first);
	}
	return animlist;
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
	string packName = ptr.toString(pack->name);

	const PartData* partDatas = ptr.toPartDatas(pack);				//array
	const AnimationData* animations = ptr.toAnimationDatas(pack);	//array

	for(int animeIndex = 0; animeIndex < pack->numAnimations; animeIndex++){
		const AnimationData* anime = &animations[animeIndex];
		const char* animeName = ptr.toString(anime->name);

		// packName + animeNameでの登録
		string animePath = toPackAnimeKey(packName, animeName);
		
		AnimeRef ref = {
			animePath, anime, partDatas, pack->numParts
		};
		m_animeRefs.insert(make_pair(animePath, ref));

		//SS_LOG("anime key: %s", key.c_str());
	}
	
}

string AnimeCache::toPackAnimeKey(const string& packName, const string& animeName)
{
	return packName + "/" + animeName;
}



} //namespace ss
