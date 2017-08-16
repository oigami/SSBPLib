#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "player/ResourceSet.h"

namespace ss{
class ResourceSet;
struct ProjectData;
class SS5Player;
class SS5Effect;
class SS5EventListener;

/**
 * SS5ResourceManager
 */
class SS5ResourceManager{
public:
	SS5ResourceManager();
	~SS5ResourceManager();

	static const std::string s_null;

	//テクスチャ事前読み込みのコールバック定義
	using PreloadCallback = std::function<void(int cellMapIndex, const std::string& texturePath, SsTexWrapMode wrapmode, SsTexFilterMode filtermode)>;


	/**
	 * ssbpファイルを登録します
	 * 登録名に依存した参照カウンタで管理されます。
	 * @param data			ssbpのデータ(中でコピーされます)
	 * @param dataSize		dataのサイズ
	 * @param dataKey		登録名
	 * @param imageBaseDir	画像ファイルの読み込み元ルートパス. 空文字のときはコンバート時に指定されたパスを使用する
	 * @param texturePreloadCallbackFunc
	 * @return { 登録後の参照カウンタの値, 不正なファイルの場合false }
	 */
	std::pair<int, bool> regist(
		const void* data,
		size_t dataSize,
		const std::string& dataKey,
		const std::string& imageBaseDir = s_null,
		PreloadCallback texturePreloadCallbackFunc = [](int, const std::string&, SsTexWrapMode, SsTexFilterMode){}
	);

	/**
	 * 指定データを解放します
	 * @param dataKey	開放したいデータの登録名
	 * @return 処理後の参照カウンタの値(0であればdeleteされています)
	 */
	int unregist(const std::string& dataKey);

	/** 全てのデータを解放します */
	void unregistAll();


	/** 指定したデータが必要とするテクスチャ名のリストを取得 */
	std::vector<std::string> getTextureList(const std::string& dataKey) const;

	/** SS5Playerの生成 */
	SS5Player* createPlayer(SS5EventListener* eventListener, const std::string& dataKey, const std::string& animeName = s_null) const;
	void destroyPlayer(SS5Player*& player) const;

	/** SS5Effectの生成 */
	SS5Effect* createEffect(SS5EventListener* eventListener, const std::string& dataKey, const std::string& effectName, int seed) const;
	void destroyEffect(SS5Effect*& effect) const;
	
private:
	//imageBaseDirの指定がないときはdataの中を見てディレクトリを返す
	std::string getImageBaseDir(const std::string& imageBaseDir, const ProjectData *data) const;

	/** 名前に対応するデータ取得します */
	const ResourceSet* getData(const std::string& dataKey) const;

	/** テクスチャ事前読み込み */
	void texturePreload(const ResourceSet* resource, PreloadCallback texturePreloadCallbackFunc) const;

private:
	/** regist数をカウントするための構造 */
	class RefcountResourceSet{
	public:
		RefcountResourceSet(const char* data, size_t dataSize, const std::string& imageBaseDir)
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

		explicit operator bool() const { return !isEmpty(); }

		bool isEmpty() const { return m_resourceSet->isEmpty(); }

	private:
		int m_refCount;
		std::unique_ptr<ResourceSet> m_resourceSet;
	};


	std::map<std::string, RefcountResourceSet*>	m_dataDic;	//ここにデータを登録する


private: //non copyable
	SS5ResourceManager(const SS5ResourceManager& o) = delete;
	SS5ResourceManager& operator=(const SS5ResourceManager& o) = delete;
};


} //namespace ss
