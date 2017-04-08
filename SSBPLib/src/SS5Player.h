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
#include <string>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include "player/UserData.h"
#include "player/State.h"
#include "player/InstancePartStatus.h"
#include "player/PlayerSetting.h"

namespace ss
{
class CustomSprite;
struct AnimeRef;
class ResourceSet;
struct ProjectData;
class Player;
class ResourceManager;
struct ResluteState;
class SS5EventListener;

//------------------------------------------------------------------------------
//プレイヤーの設定定義
//使用するアニメーションに合わせて設定してください。


//プレイヤーで扱えるアニメに含まれるパーツの最大数
#define PART_VISIBLE_MAX (512)

//座標系の設定：上方向がマイナスの場合はscaleYを-1にしてください

//------------------------------------------------------------------------------



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


	/** 再生を再開します. */
	void resume();

	/** 再生を停止します. */
	void stop();

	/** 再生しているアニメーションのパック名(ssae)を返します. */
	std::string getPlayPackName() const;

	/** 再生しているアニメーション名を返します. */
	std::string getPlayAnimeName() const;
	
	/** アニメーションの総フレームを取得します. */
	int getMaxFrame() const;

	/** 再生フレームNoを取得します. */
	int getCurrentFrame() const;

	/** 再生フレームNoを設定します. */
	void setCurrentFrame(int frame);


	/** ラベル名からフレーム位置を取得します. */
	int getLabelToFrame(const std::string& labelName) const;

	/** 再生しているアニメーションに含まれるパーツ数を取得します. */
	int getPartsCount() const;

	/** indexからパーツ名を取得します */
	std::string getPartName(int partIndex) const;

	/** パーツ名からindexを取得します */
	int indexOfPart(const std::string& partName) const;

	/**
	 * パーツ情報を取得します.
	 * @param result	パーツ情報を受け取るバッファ
	 * @param name		取得するパーツのインデックス
	 */
	void getPartState(ResluteState& result, int partIndex) const;

	
	/** 指定したパーツを表示、非表示を設定します */
	void setPartVisible(int partIndex, bool visible);

	/**
	 * パーツ名からパーツに割り当たるセルを変更します.
	 * この関数で設定したパーツは参照セルアトリビュートの影響をうけません。
	 * アニメに設定されたセルに戻す場合は、セル名に""を指定してください。
	 * @param partIndex	パーツ名
	 * @param sscename	セルマップ名
	 * @param cellname	表示させたいセル名
	 */
	void setPartCell(int partIndex, const std::string& sscename, const std::string& cellname);


	/** プレイヤー本体に最初に掛ける行列を設定します */
	void setRootMatrix(const Matrix& matrix);

	/** プレイヤー本体の位置を設定します */
	void  setPosition(float x, float y);

	/** プレイヤー本体の回転角度を設定します。2Dの回転はZに値を設定してください。 */
	void  setRotation(float x, float y, float z);
	/** プレイヤー本体のスケールを設定します。*/
	void  setScale(float x, float y);

	/** プレイヤー本体の透明度を設定します[0:1]*/
	void  setAlpha(float a);

	/**
	 * アニメの色設定
	 * @param r		赤成分[0:1]
	 * @param g		緑成分[0:1]
	 * @param b		青成分[0:1]
	 */
	void setColor(float r, float g, float b);


	/** プレイヤーの更新を行います。ゲームの更新タイミングで呼び出してください。*/
	void update(float dt);

	/** プレイヤーの表示を行います。ゲームの表示タイミングで呼び出してください。*/
	void draw();


private:
	void allocParts(int numParts);
	void releaseParts();
	void setPartsParentage();

	void play(const AnimeRef* animeRef, int startFrameNo);
	void setFrame(int frameNo);
	void checkUserData(int frameNo);

private:
	SS5EventListener* _eventListener;
	const ResourceSet* _resource;			//ssbp
	const AnimeRef*	_animationData;			//アニメーションデータ
	
	std::vector<CustomSprite *> _parts;
	std::vector<TextureID> m_textures;		//テクスチャ
	
	float				_currentFrameTime;		//現在のフレーム。小数点を考慮するが、フレームに直すには単にintにすれば良い
	bool				_isPausing;
	bool				_partVisible[PART_VISIBLE_MAX];
	int					_cellChange[PART_VISIBLE_MAX];
	int					_partIndex[PART_VISIBLE_MAX];
	int					_seedOffset;					//エフェクトシードオフセット

	PlayerSetting		_playerSetting;			//プレイヤー単位での操作設定についてを抱えておく(移動、回転など)


	int getAnimeFPS() const;
};


} // namespace ss
