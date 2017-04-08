#pragma once
#include <string>
#include <map>
#include <memory>
#include "player/ResourceSet.h"

namespace ss{
class ResourceSet;
struct ProjectData;
class Player;
class SS5Effect;
class SS5EventListener;

/**
 * ResourceManager
 */
class ResourceManager{
public:
	ResourceManager();
	~ResourceManager();

	static const std::string s_null;


	/**
	 * ssbpファイルを登録します
	 *
	 * @param  data			ssbpのデータ(中でコピーされます)
	 * @param  dataSize		dataのサイズ
	 * @param  dataKey		登録名
	 * @param  imageBaseDir 画像ファイルの読み込み元ルートパス. 省略時はコンバート時に指定されたパスを使用する
	 */
	int regist(const void *data, size_t dataSize, const std::string &dataKey, const std::string &imageBaseDir = s_null);

	 /** 指定データを解放します。登録名を指定してください */
	void unregist(const std::string& dataKey);

	/** 全てのデータを解放します */
	void unregistAll();


	/** 指定したデータが必要とするテクスチャ名のリストを取得 */
	void getTextureList(std::vector<std::string> *textureList, const std::string &dataKey) const;

	/** SS5Playerの生成 */
	Player* createPlayer(SS5EventListener* eventListener, const std::string& dataKey, const std::string& animeName) const;
	void destroyPlayer(Player *&player) const;

	/** SS5Effectの生成 */
	SS5Effect* createEffect(SS5EventListener* eventListener, const std::string& dataKey, const std::string& effectName, int seed) const;
	void destroyEffect(SS5Effect*& effect) const;
	
private:
	//imageBaseDirの指定がないときはdataの中を見てディレクトリを返す
	std::string getImageBaseDir(const std::string &imageBaseDir, const ProjectData *data) const;

	/** 名前に対応するデータ取得します */
	const ResourceSet* getData(const std::string& dataKey) const;

private:
	/** regist数をカウントするための構造 */
	class RefcountResourceSet{
	public:
		RefcountResourceSet(const char *data, size_t dataSize, const std::string &imageBaseDir)
			: m_refCount(0), m_resourceSet(new ResourceSet(data, dataSize, imageBaseDir)){
			incCount();
		}
		~RefcountResourceSet(){}

		int getCount() const{ return m_refCount; }
		void incCount(){ ++m_refCount; }
		void decCount(){ --m_refCount; }

		ResourceSet* getResourceSet() const{
			return m_resourceSet.get();
		}

	private:
		int m_refCount;
		std::unique_ptr<ResourceSet> m_resourceSet;
	};


	std::map<std::string, RefcountResourceSet*>	_dataDic;	//ここにデータを登録する


private: //non copyable
	ResourceManager(const ResourceManager &o) = delete;
	ResourceManager& operator=(const ResourceManager &o) = delete;
};

} //namespace ss
