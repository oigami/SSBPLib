#pragma once
#include <string>
#include <vector>
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

	
	/** "packname/animename"で指定してAnimeRefを得る */
	const AnimeRef* getReference(const std::string& animeName) const;


	/** 先頭のアニメーション名を返す */	//memo:SS5Playerの初期化で必要
	const std::string& getFirstAnimationName() const;
	
	/** アニメーションの登録名のリストを取得する。主にデバッグ用 */
	std::vector<std::string> getAnimationList() const;

private:
	/** dataを元にAnimeRefを構築 */
	void init(const ProjectData* data);
	void addAnimationData(ToPointer ptr, const AnimePackData* pack);

	/** ファイルパス生成 */
	static std::string toPackAnimeKey(const std::string& packName, const std::string& animeName);


private:
	std::map<std::string, AnimeRef> m_animeRefs;	// <"packname/animename", AnimeRef>
};


} //namespace ss
