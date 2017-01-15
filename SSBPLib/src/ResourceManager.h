#pragma once
#include <string>
#include <map>
#include <memory>
#include "player/ResourceSet.h"

namespace ss{
class ResourceSet;
struct ProjectData;

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

#if 0
	/**
	 * 指定したセルのテクスチャを変更します.
	 * @param  ssbpName       ssbp名（拡張子を除くファイル名）
	 * @param  ssceName       ssce名（拡張子を除くファイル名）
	 * @param  texture        変更後のテクスチャハンドル
	 * @return 変更を行ったか
	 */
	bool changeTexture(char* ssbpName, char* ssceName, long texture);

	/**
	 * 指定したデータのテクスチャを破棄します。
	 * @param  dataName       ssbp名（拡張子を除くファイル名）
	 * @return 成功失敗
	 */
	bool releseTexture(char* ssbpName);

	/**
	 * 読み込んでいるssbpからアニメーションの総フレーム数を取得します。
	 * @param  ssbpName       ssbp名（拡張子を除くファイル名）
	 * @param  animeName      ssae/モーション名
	 * @return アニメーションの総フレーム（存在しない場合はアサート）
	 */
	int getMaxFrame(std::string ssbpName, std::string animeName);
#endif

private:
	//imageBaseDirの指定がないときはdataの中を見てディレクトリを返す
	std::string getImageBaseDir(const std::string &imageBaseDir, const ProjectData *data) const;
public:
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
