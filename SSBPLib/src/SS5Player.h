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
  ssplayer->setAlpha(255);
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

#ifndef SS5Player_h
#define SS5Player_h

#include "SS5PlayerData.h"
#include "SS5PlayerTypes.h"
#include "SS5PlayerPlatform.h"
#include <map>
#include <vector>
#include <string>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include "player/UserData.h"
#include "player/State.h"

//エフェクト関連
#include "Common/loader/ssloader.h"
#include "Common/Animator/ssplayer_macro.h"
#include "Common/Animator/ssplayer_matrix.h"
#include "Common/Animator/ssplayer_effectfunction.h"
#include "Common/Animator/ssplayer_cellmap.h"
#include "Common/Animator/ssplayer_PartState.h"
//#include "Common/Animator/MersenneTwister.h"


namespace ss
{
class CustomSprite;
struct AnimeRef;
class ResourceSet;
struct ProjectData;
class Player;
class ResourceManager;
struct ResluteState;


//------------------------------------------------------------------------------
//プレイヤーの設定定義
//使用するアニメーションに合わせて設定してください。


//プレイヤーで扱えるアニメに含まれるパーツの最大数
#define PART_VISIBLE_MAX (512)

//座標系の設定：上方向がマイナスの場合はscaleYを-1にしてください

//------------------------------------------------------------------------------


/** Playerが再生するパーツ全体に影響を与える設定を抱えておく */
struct PlayerSetting{
	PlayerSetting() : m_opacity(255), m_col_r(255), m_col_g(255), m_col_b(255){}

	Vector3 m_position;	//位置
	Vector3 m_rotation;	//回転(deg)
	Vector3 m_scale;	//スケール
	
	int     m_opacity;	//不透明度[0:255]
	int		m_col_r;
	int		m_col_g;
	int		m_col_b;

	void getTransformMatrix(Matrix* matrix) const{
		Matrix tmp;
		matrix->setupIdentity();
		*matrix *= tmp.setupScale(m_scale.x, m_scale.y, m_scale.z);
		*matrix *= tmp.setupRotationZ(DegreeToRadian(m_rotation.z));
		*matrix *= tmp.setupRotationY(DegreeToRadian(m_rotation.y));
		*matrix *= tmp.setupRotationX(DegreeToRadian(m_rotation.x));
		*matrix *= tmp.setupTranslation(m_position.x, m_position.y, m_position.z);
	}
};



/**
 * Player
 */
class Player{
public:
	/** Playerインスタンスを構築します。利用するときはResourceManger::create, destroyを使ってください */
	Player(const ResourceSet *resource/*, SS5Renderer *renderer, SS5EventListener *eventListener*/);
//	Player(const ResourceSet *resource/*, SS5Renderer *renderer, SS5EventListener *eventListener, const std::string& animeName*/);
	~Player();	//memo:なるべくResourceManger.create, destroyを使ってほしい

	/**
	 * アニメーションの再生を開始します.
	 *
	 * @param  ssaeName      パック名(ssae名）
	 * @param  motionName    再生するモーション名
	 * @param  loop          再生ループ数の指定. 省略時は0
	 * @param  startFrameNo  再生を開始するフレームNoの指定. 省略時は0
	 */
	void play(const std::string& ssaeName, const std::string& motionName, int loop = 0, int startFrameNo = 0);

	/**
	 * アニメーションの再生を開始します.
	 * アニメーション名から再生するデータを選択します.
	 * "ssae名/モーション名で指定してください.
	 * sample.ssaeのanime_1を指定する場合、sample/anime_1となります.
	 * ※ver1.1からモーション名のみで指定する事はできなくなりました。
	 *
	 * @param  animeName     再生するアニメーション名
	 * @param  loop          再生ループ数の指定. 省略時は0
	 * @param  startFrameNo  再生を開始するフレームNoの指定. 省略時は0
	 */
	void play(const std::string& animeName, int loop = 0, int startFrameNo = 0);

	/**
	 * 現在再生しているモーションとブレンドしながら再生します。
	 * アニメーション名から再生するデータを選択します.
	 * "ssae名/モーション名で指定してください.
	 * sample.ssaeのanime_1を指定する場合、sample/anime_1となります.
	 * ※ver1.1からモーション名のみで指定する事はできなくなりました。
	 *
	 * ブレンドするアニメーションの条件は以下になります。
	 * ・同じssbp内に含まれている事
	 * ・同じパーツ構成（パーツ順、パーツ数）である事
	 * SpriteStudioのフレームコントロールに並ぶパーツを上から順にブレンドしていきます。
	 * パーツ名等のチェックは行なっていませんので遷移元と遷移先アニメのパーツの順番を同じにする必要があります。
	 * 遷移元と遷移先のパーツ構成があっていない場合、正しくブレンドされませんのでご注意ください。
	 *
	 * 合成されるアトリビュートは
	 * 座標X、座標Y、X回転、Y回転、Z回転、スケールX、スケールYのみです。
	 * それ以外のアトリビュートは遷移先アニメの値が適用されます。
	 * インスタンスパーツが参照しているソースアニメはブレンドされません。
	 * エフェクトパーツから発生したパーティクルはブレンドされません。
	 * 
	 *
	 * @param  animeName     再生するアニメーション名
	 * @param  loop          再生ループ数の指定. 省略時は0
	 * @param  startFrameNo  再生を開始するフレームNoの指定. 省略時は0
	 * @param  blendTime		モーションブレンドを行う時間、単位は秒　省略時は1秒
	 */
	void motionBlendPlay(const std::string& animeName, int loop = 0, int startFrameNo = 0, float blendTime = 0.1f);

	/** 再生を中断します. */
	void animePause();

	/** 再生を再開します. */
	void animeResume();

	/**
	 * 再生を停止します.
	 * ゲーム側でアニメーションの表示フレームを制御する場合はstop()を呼び出した後
	 * ゲーム側の更新処理でsetFrameNo()を呼び出し指定のフレームを表示してください。
	 */
	void stop();

	/** 再生しているアニメーションのパック名(ssae)を返します. */
	const std::string& getPlayPackName() const;

	/** 再生しているアニメーション名を返します. */
	const std::string& getPlayAnimeName() const;
	
	/** アニメーションの総フレームを取得します. */
	int getMaxFrame() const;

	/** 再生フレームNoを取得します. */
	int getFrameNo() const;

	/** 再生フレームNoを設定します. */
	void setFrameNo(int frameNo);
	
	/** 指定されている再生ループ回数を取得します. (0:指定なし) */
	int getLoop() const;

	/** 再生ループ回数を設定します. (0:指定なし) */
	void setLoop(int loop);

	/** 現在までのループ再生回数を取得します. */
	int getLoopCount() const;

	/** 現在までのループ再生回数をクリアします. */
	void clearLoopCount();

	/** フレームスキップ（フレームレートに合わせ再生フレームをスキップする）の設定をします. (default: true) */
	void setFrameSkipEnabled(bool enabled);
	
	/** フレームスキップの設定状態を返します. */
	bool isFrameSkipEnabled() const;

	/** ラベル名からフレーム位置を取得します. */
	int getLabelToFrame(char* findLabelName);

	/** 再生しているアニメーションに含まれるパーツ数を取得します. */
	int getPartsCount(void);

	/**
	 * indexからパーツ名を取得します.	//todo:コメントのparamが謎なのであとで整理する
	 *
	 * @param  result        パーツ情報を受け取るバッファ
	 * @param  name          取得するパーツ名
	 * @param  frameNo       取得するフレーム番号 -1の場合は現在再生しているフレームが適用される
	 */
	const char* getPartName(int partId) const;

	/** パーツ名からindexを取得します. */
	int indexOfPart(const char* partName) const;

	/**
	 * パーツの名から、パーツ情報を取得します.
	 *
	 * @param  result        パーツ情報を受け取るバッファ
	 * @param  name          取得するパーツ名
	 * @param  frameNo       取得するフレーム番号 -1の場合は現在再生しているフレームが適用される
	 */
	bool getPartState(ResluteState& result, const char* name, int frameNo = -1);

	/**
	 * パーツ名からパーツの表示、非表示を設定します.
	 * コリジョン用のパーツや差し替えグラフィック等、SS5上で表示を行うがゲーム中では非表示にする場合に使用します。
	 * SSの非表示アトリビュート設定するわけではないので注意してください。
	 */
	void setPartVisible(std::string partsname, bool flg);

	/**
	 * パーツ名からパーツに割り当たるセルを変更します.
	 * この関数で設定したパーツは参照セルアトリビュートの影響をうけません。
	 * アニメに設定されたセルに戻す場合は、セル名に""を指定してください。
	 *
	 * @param  partsname         パーツ名
	 * @param  sscename          セルマップ名
	 * @param  cellname          表示させたいセル名
	 */
	void setPartCell(std::string partsname, std::string sscename, std::string cellname);

	/** プレイヤー本体の位置を設定します */
	void  setPosition(float x, float y);

	/** プレイヤー本体の回転角度を設定します。2Dの回転はZに値を設定してください。 */
	void  setRotation(float x, float y, float z);
	/** プレイヤー本体のスケールを設定します。*/
	void  setScale(float x, float y);

	/** プレイヤー本体の透明度を設定します。*/
	void  setAlpha(int a);

	/**
	 * アニメの輝度を設定します.
	 * setColor(Color3B)ではなくこちらを使用してください。
	 * 制限としてカラーブレンドが適用されたパーツの色は変更できませんので注意してください。
	 *
	 * @param  r          赤成分(0～255)
	 * @param  g          緑成分(0～255)
	 * @param  b          青成分(0～255)
	 */
	void setColor(int r, int g, int b);

	/**
	 * 名前を指定してパーツの再生するインスタンスアニメを変更します。
	 * 指定したパーツがインスタンスパーツでない場合、falseを返します.
	 * インスタンスパーツ名はディフォルトでは「ssae名:モーション名」とつけられています。
	 * 再生するアニメの名前は"ssae名/アニメーション名"として再生してください。
	 * 現在再生しているアニメを指定することは入れ子となり無限ループとなるためできません。
	 * 変更するアニメーションは同じssbpに含まれる必要があります。
	 *
	 * インスタンスキーを手動で設定する事が出来ます。
	 * アニメーションに合わせて開始フレーム、終了フレーム等のインスタンスアトリビュート情報を設定してください。
	 * 終了フレーム最大値は総フレーム-1になります。
	 * 上書きフラグがfalseの場合、SS上に設定されたインスタンスアトリビュートの設定を使用します。
	 * 使用例：
	 * ss::Instance param;
	 * param.clear();
	 * param.refEndframe = resman->getMaxFrame("ssbp名","ssae名/モーション名") - 1;	//アニメーションの長さを取得
	 * param.infinity = true;														//無限ループを設定
	 * ssplayer->changeInstanceAnime("再生しているアニメーションに含まれるインスタンスパーツ名", "ssae名/モーション名", true, param);
	 *
	 * @param  partsname			SS上のパーツ名
	 * @param  animeName			参照するアニメ名
	 * @param  overWrite			インスタンスキーの上書きフラグ
	 * @param  keyParam			インスタンスキーのパラメータ
	 */
	bool changeInstanceAnime(std::string partsname, std::string animeName, bool overWrite, Instance keyParam);

	/**
	 * プレイヤーにインスタンスパラメータを設定します。
	 * @param  overWrite		インスタンスキーの上書きフラグ
	 * @param  keyParam			インスタンスキーのパラメータ
	 */
	void setInstanceParam(bool overWrite, Instance keyParam);

	/**
	 * プレイヤーからインスタンスパラメータを取得します。
	 *
	 * @param  overWrite		インスタンスキーの上書きフラグ
	 * @param  keyParam			インスタンスキーのパラメータ
	 */
	void getInstanceParam(bool *overWrite, Instance *keyParam);

	/**
	 * アニメーションのループ範囲（再生位置）を上書きします。
	 *
	 * @param  frame			開始フレーム（-1で上書き解除）
	 */
	void setStartFrame(int frame);

	/*
	 * アニメーションのループ範囲（終了位置）を上書きします。
	 * SpriteStudioのフレーム数+1を設定してください。
	 *
	 * @param  frame			終了フレーム（-1で上書き解除）
	 */
	void setEndFrame(int frame);

	/*
	 * アニメーションのループ範囲（再生位置）を上書きします。
	 *
	 * @param  labelname			開始フレームとなるラベル名（""で上書き解除）
	 */
	void setStartFrameToLabelName(char *findLabelName);

	/*
	 * アニメーションのループ範囲（終了位置）を上書きします。
	 *
	 * @param  labelname			終了フレームとなるラベル名（""で上書き解除）
	 */
	void setEndFrameToLabelName(char *findLabelName);


	/*
	 * パーツ番号に対応したスプライト情報を取得します。
	 * @param  partIndex			パーツ番号
	 */
	const CustomSprite* getSpriteData(int partIndex) const;

	/** 表示を行うパーツ数を取得します */
	int getDrawSpriteCount(void);

	/** プレイヤーの更新を行います。ゲームの更新タイミングで呼び出してください。*/
	void update(float dt);

	/** プレイヤーの表示を行います。ゲームの表示タイミングで呼び出してください。*/
	void draw();


private:
	void allocParts(int numParts, bool useCustomShaderProgram);
	void releaseParts();
	void setPartsParentage();

	void play(AnimeRef* animeRef, int loop, int startFrameNo);
	void setFrame(int frameNo, float dt = 0.0f);
	void checkUserData(int frameNo);
	float parcentValRot(float val1, float val2, float parcent);

private:
	const ResourceSet*		_currentRs;
	std::string			_currentAnimename;
	AnimeRef*			_currentAnimeRef;
	std::vector<CustomSprite *>	_parts;

	Player*				_motionBlendPlayer;
	float				_blendTime;
	float				_blendTimeMax;

	bool				_frameSkipEnabled;
	float				_playingFrame;
	int					_loop;
	int					_loopCount;
	bool				_isPlaying;
	bool				_isPausing;
	bool				_isPlayFirstUserdataChack;
	int					_prevDrawFrameNo;
	bool				_partVisible[PART_VISIBLE_MAX];
	int					_cellChange[PART_VISIBLE_MAX];
	int					_partIndex[PART_VISIBLE_MAX];
	bool				_instanceOverWrite;				//インスタンス情報を上書きするか？
	Instance			_instanseParam;					//インスタンスパラメータ
	int					_startFrameOverWrite;			//開始フレームの上書き設定
	int					_endFrameOverWrite;				//終了フレームの上書き設定
	int					_seedOffset;					//エフェクトシードオフセット
	int					_draw_count;					//表示スプライト数

	UserData			_userData;

	PlayerSetting		_playerSetting;			//プレイヤー単位での操作設定についてを抱えておく(移動、回転など)

	std::vector<TextuerData> m_textures;		//_textures[cellMapIndex].handle = textureid;

	int getAnimeFPS() const;
};


};	// namespace ss

#endif
