#pragma once
#include <string>
#include <map>

namespace ss{
struct AnimationData;
struct AnimePackData;
struct ProjectData;

/**
 * AnimeRef
 */
struct AnimeRef{
	std::string				m_packName;
	std::string				m_animeName;
	const AnimationData*	m_animationData;
	const AnimePackData*	m_animePackData;
};


/**
 * AnimeCache
 */
class AnimeCache{
public:
	AnimeCache(const ProjectData* data);
	~AnimeCache();

	/** packNameとanimeNameを指定してAnimeRefを得る	*/
	AnimeRef* getReference(const std::string& packName, const std::string& animeName);

	/** animeNameのみ指定してAnimeRefを得る	*/
	AnimeRef* getReference(const std::string& animeName);

	void dump();

private:
	void init(const ProjectData* data);

	static std::string toPackAnimeKey(const std::string& packName, const std::string& animeName);

	//キャッシュの削除
	void releseReference(void);

	std::map<std::string, AnimeRef*> m_animeRefs;
};

} //namespace ss
