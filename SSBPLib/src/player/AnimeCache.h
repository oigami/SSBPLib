#pragma once
#include <string>
#include <map>
#include "SS5PlayerData.h"
#include "Util.h"


namespace ss{
struct ProjectData;
struct AnimationData;
struct AnimePackData;
struct PartData;
class ToPointer;

/**
 * AnimeRef
 *  AnimationDataへのアクセスを提供
 *  パーツデータはssae単位で共通のものだが、利便性のためここで抱えておく
 */
struct AnimeRef{
	std::string				m_packName;			//todo: const char*
	std::string				m_animeName;		//todo: const char*
	const AnimationData*	m_animationData;	//アニメーション

	//const AnimePackData*	m_animePackData;	//属するパッケージ
	const PartData*			m_partDatas;		//パーツデータへのショートカット aniemPackData->partDatas[]
	int						m_numParts;			//パーツデータ数

	//PartDataへのアクセサ
	const PartData* getPartData(int partIndex) const{
		SS_ASSERT_LOG(partIndex >= 0 && partIndex < m_numParts, "partId is out of range.");
		return &m_partDatas[partIndex];
	}
};


/**
 * AnimeCache
 *  ProjectDataからAnimationDataへのアクセスを構築する
 */
class AnimeCache{
public:
	/** dataを元にAnimeRefを構築 */
	AnimeCache(const ProjectData* data);
	~AnimeCache();
#if 0
	/** packNameとanimeNameを指定してAnimeRefを得る */
	const AnimeRef* getReference(const std::string& packName, const std::string& animeName) const;
#endif
	/** animeNameのみ指定してAnimeRefを得る */
	const AnimeRef* getReference(const std::string& animeName) const;

	/** デバッグ用 */
	void dump() const;

private:
	/** dataを元にAnimeRefを構築 */
	void init(const ProjectData* data);
	void addAnimationData(ToPointer ptr, const AnimePackData* pack);

	/** ファイルパス生成 */
	static std::string toPackAnimeKey(const std::string& packName, const std::string& animeName);


private:
	std::map<std::string, AnimeRef> m_animeRefs;
};


} //namespace ss
