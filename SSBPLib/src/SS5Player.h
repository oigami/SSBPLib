#pragma once
//-----------------------------------------------------------
// ssbpLib v1.3.5
//
// Copyright(C) Web Technology Corp.
// http://www.webtech.co.jp/
//-----------------------------------------------------------
//
// SS5Player.h
//


/************************************************************
対応するssbpフォーマットはバージョン4です。
Ss5ConverterのフォーマットバージョンはSpriteStudioSDKを参照してください。
https://github.com/SpriteStudio/SpriteStudio5-SDK/wiki/%E3%82%B3%E3%83%B3%E3%83%90%E3%83%BC%E3%82%BF%E3%81%AE%E4%BD%BF%E3%81%84%E6%96%B9

- Quick start
 
  #include "SS5Player.h"

  
  // SS5プレイヤーの宣言
  ss::Player *ssplayer;
  ss::ResourceManager *resman;


  //リソースマネージャの作成
  resman = ss::ResourceManager::getInstance();
  //プレイヤーの作成
  ssplayer = ss::Player::create();

  //アニメデータをリソースに追加
  //それぞれのプラットフォームに合わせたパスへ変更してください。
  resman->addData("character_template_comipo\\character_template1.ssbp");
  //プレイヤーにリソースを割り当て
  ssplayer->setData("character_template1");					// ssbpファイル名（拡張子不要）
  //再生するモーションを設定
  ssplayer->play("character_template_3head/stance");		// アニメーション名を指定(ssae名/アニメーション名)


  //表示位置を設定
  ssplayer->setPosition(1280/2, 720);
  //スケール設定
  ssplayer->setScale(0.5f, 0.5f);
  //回転を設定
  ssplayer->setRotation(0.0f, 0.0f, 0.0f);
  //透明度を設定
  ssplayer->setAlpha(1.0f);
  //反転を設定
  ssplayer->setFlip(false, false);


  //メインループで呼び出してください。
  ssplayer->update(dt);					//プレイヤーの更新
  ssplayer->draw();						//プレイヤーの描画


  終了処理で resman、player を delete してください。
  //SS5Playerの削除
  delete (ssplayer);
  delete (resman);

  使用するアニメーションに合わせて Playerクラス定義部分にある設定用定数を変更してください。

  ssbpLibの制限についてはこちらのページを参照してください。
  https://github.com/SpriteStudio/ssbpLib/wiki

  使用方法についてはPlayerクラスのコメントを参照してください。

*************************************************************/

#include "SS5PlayerData.h"
#include "SS5PlayerTypes.h"
#include <map>
#include <vector>
#include <deque>
#include <string>
#include <list>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include "player/UserData.h"
#include "player/State.h"
#include "player/InstancePartStatus.h"
#include "player/PlayerSetting.h"

namespace ss{
class CustomSprite;
struct AnimeRef;
class ResourceSet;
struct ProjectData;
class Player;
class ResourceManager;
struct ResluteState;
class SS5EventListener;


/**
 * Player
 */
class Player{
public:
	/** Playerインスタンスを構築します。利用するときはResourceManger::create, destroyを使ってください */
	Player(SS5EventListener* eventListener, const ResourceSet* resource, const std::string& animeName);
	~Player();	//memo:なるべくResourceManger.create, destroyを使ってほしい

	/**
	 * アニメーションの再生を開始します.
	 * アニメーション名から再生するデータを選択します.
	 * "ssae名/モーション名"で指定してください.
	 * sample.ssaeのanime_1を指定する場合、sample/anime_1となります.
	 * @param  animeName     再生するアニメーション名
	 * @param  startFrameNo  再生を開始するフレームNoの指定. 省略時は0
	 */
	void play(const std::string& animeName, int startFrameNo = 0);


	/* 毎フレーム呼ぶもの */
	void update(float dt);		//更新
	void draw();				//描画


	/* 再生フレーム */
	int getMaxFrame() const;			//アニメーションの総フレームを取得
	int getCurrentFrame() const;		//再生フレームを取得
	void setCurrentFrame(int frame);	//再生フレームの設定。update呼ぶまで反映されません

	/* パーツへのアクセサ */
	int getPartsNum() const;										//再生しているアニメーションに含まれるパーツ数を取得
	std::string getPartName(int partIndex) const;					//indexからパーツ名を取得
	int indexOfPart(const std::string& partName) const;				//パーツ名からindexを取得
	void getPartState(ResluteState& result, int partIndex) const;	//パーツ情報を取得
	int getLabelToFrame(const std::string& labelName) const;		//ラベル名からフレーム位置を取得

	
	/** 指定したパーツを表示、非表示を設定します */
	void setPartVisible(int partIndex, bool visible);

	/**
	 * パーツ名からパーツに割り当たるセルを変更します.
	 * この関数で設定したパーツは参照セルアトリビュートの影響をうけません。
	 * アニメに設定されたセルに戻す場合は、セル名に""を指定してください。
	 * @param partIndex	パーツ名
	 * @param cellname	表示させたいセル名("セルマップ名/セル名"の形式)
	 */
	void setPartCell(int partIndex, const std::string& cellname);


	/** プレイヤー本体に最初に掛ける行列を設定します */
	void setRootMatrix(const Matrix& matrix);

	/** プレイヤー本体の位置を設定します */
	void setPosition(float x, float y);
	void getPosition(float* x, float* y) const;				//rootMatrix無視

	/** プレイヤー本体の回転角度を設定します。2Dの回転はZに値を設定してください。 */
	void setRotation(float x, float y, float z);
	void getRotation(float* x, float* y, float* z) const;	//rootMatrix無視
	/** プレイヤー本体のスケールを設定します。*/
	void setScale(float x, float y);						//rootMatrix無視
	void getScale(float* x, float* y) const;

	/** プレイヤー本体の透明度を設定します[0:1]*/
	void setAlpha(float a);
	float getAlpha() const;

	/**
	 * アニメの色設定
	 * @param r		赤成分[0:1]
	 * @param g		緑成分[0:1]
	 * @param b		青成分[0:1]
	 */
	void setColor(float r, float g, float b);




	/** 再生可能なアニメーションのリストを返す */
	void getAnimationList(std::list<std::string> *animlist) const;

private:
	void allocParts(int numParts);
	void releaseParts();
	void setPartsParentage();

	void play(const AnimeRef* animeRef, int startFrameNo);
	void setFrame(int frameNo);
	void checkUserData(int frameNo);

	int getAnimeFPS() const;
	
private:
	SS5EventListener*  m_eventListener;
	const ResourceSet* m_resource;			//ssbp
	const AnimeRef*	   m_animationData;		//アニメーションデータ
	std::vector<TextureID> m_textures;		//テクスチャ

	std::vector<CustomSprite> m_parts;
	std::vector<int> m_drawOrderIndex;	//描画優先順でpartIndexを保存しておく  priority: [0] > [max]

	//書き換え用。アニメーションが切り替わっても設定は維持されます
	std::deque<bool> m_partVisible;		//visibleの上書き。デフォルトはtrue。vector<bool>は特殊化されるのでdequeで代用する
	std::vector<int> m_changeCellIndex;	//cellの差し替わり番号 未使用時は-1
	
	
	float m_currentFrameTime;		//現在のフレーム。小数点を考慮するが、フレームに直すには単にintにすれば良い
	int m_seedOffset;				//エフェクトシードオフセット

	PlayerSetting m_playerSetting;	//プレイヤー単位での操作設定についてを抱えておく(移動、回転など)


private: //non copyable
	Player(const Player &o) = delete;
	Player& operator=(const Player &o) = delete;
};

using SS5Player = Player;

} // namespace ss
