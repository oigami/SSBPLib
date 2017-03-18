﻿#include "DxLib.h"
#include <fstream>
#include "SS5Player.h"
#include "ResourceManager.h"
#include "ResluteState.h"
#include "SS5EventListener.h"

//メモリリークチェック用---------------------------------------------------------
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
//-----------------------------------------------------------------------------

static int previousTime;
static int waitTime;
int mGameExec;

#define WAIT_FRAME (16)

void init(void);
void update(float dt);
void draw(void);
void relese(void);

/// SS5プレイヤー
ss::Player *ssplayer;
ss::ResourceManager *resman;

/*イベントリスナーの実装*/
class SimpleSS5EventListener: public ss::SS5EventListener{
public:
	void onUserData(ss::Player* player, const ss::UserData& userData, int frameNo) override{}	//ユーザーデータは特に何も処理しない
	int limitFrame(ss::Player* player, int frame, int maxFrame) override{
		return ss::SS5EventListener::limitFrame(player, frame, maxFrame);	//フレーム制限はデフォルト実装に任せる
	}

	//テクスチャのロード・リリースのイベント。内部ではPlayer単位で管理されます
	ss::TextureID SSTextureLoad(const char* pszFileName, ss::SsTexWrapMode::_enum wrapmode, ss::SsTexFilterMode::_enum filtermode) override{
		return LoadGraph(pszFileName);
	}
	void SSTextureRelease(ss::TextureID handle) override{
		DeleteGraph(handle);
	}

	//テクスチャサイズの取得
	void SSGetTextureSize(ss::TextureID handle, int* width, int* height) override{
		GetGraphSize(handle, width, height);
	}


	//インスタンスアニメーションのイベント
	bool ChildPlayerLoad(int parentPartIndex, const std::string& parentPartName, const std::string& animName){ return false; }
	void ChildPlayerRelease(int parentPartIndex, const std::string& parentPartName){}
	void ChildPlayerSetFrame(
		int parentPartIndex, const std::string& parentPartName,
		const ss::ParentPartState& parentPartState, int frame, bool independent
	)override{}
	void ChildPlayerDraw(int parentPartIndex, const std::string& parentPartName){}

};
SimpleSS5EventListener g_eventListener;


//ファイル読み込みして中身を返します
std::vector<char> readfile(const std::string& filename)
{
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	assert(ifs);

	ifs.seekg(0, std::fstream::end);
	size_t filesize = ifs.tellg();
	ifs.seekg(0, std::fstream::beg);

	std::vector<char> buf(filesize, 0);
	ifs.read(buf.data(), filesize);
	return buf;
}


/**
* メイン関数
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//メモリリークチェック用
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

	//DXライブラリの初期化
	ChangeWindowMode(true);	//ウインドウモード
	SetGraphMode(800, 600, GetColorBitDepth() );
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	SetDrawScreen(DX_SCREEN_BACK);

	//メインループ
	mGameExec = 1;
	previousTime = GetNowCount();
	
	// プレイヤー初期化
	init( );
	
	while(mGameExec == 1){
		ClearDrawScreen();
		update((float)waitTime / 1000.0f );		//ゲームの更新
		draw();									//ゲームの描画
		ScreenFlip();							//描画結果を画面に反映

		//次のフレームまでの時間待ち
		waitTime = GetNowCount() - previousTime;
		previousTime = GetNowCount();

		if (waitTime < WAIT_FRAME){
			WaitTimer((WAIT_FRAME - waitTime));
		}else{
			if(ProcessMessage() == -1) mGameExec = 0;
		}
	}

	/// プレイヤー終了処理
	relese( );


	DxLib_End();			// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

void init( void )
{
	/**********************************************************************************

	SSアニメ表示のサンプルコード
	Visual Studio Community、DXライブラリで動作を確認しています。
	ssbpとpngがあれば再生する事ができますが、Resourcesフォルダにsspjも含まれています。

	**********************************************************************************/
	//リソースマネージャの作成
	resman =  new ss::ResourceManager();
	

	//ssbpファイルの読み込み
	std::vector<char> buf = readfile("Resources/character_template_comipo/character_template1.ssbp");

	//アニメデータをリソースに追加
	resman->regist(
		buf.data(), buf.size(),
		"character_template1",					//登録名
		"Resources/character_template_comipo/"	//画像ファイルの読み込み元ルートパス
	);
	//プレイヤーにリソースを割り当て
	ssplayer = resman->createPlayer("character_template1", &g_eventListener);       //addDataで指定した登録名
	//再生するモーションを設定
	ssplayer->play("character_template_3head/stance");				 // アニメーション名を指定(ssae名/アニメーション名も可能、詳しくは後述)


	//表示位置を設定
	ssplayer->setPosition(800/2, 150);
	//スケール設定	//反転させたいときは-の値を指定してください
	ssplayer->setScale(0.5f, 0.5f);
	//回転を設定
	ssplayer->setRotation(0.0f, 0.0f, 0.0f);
	//透明度を設定
	ssplayer->setAlpha(255);


	//エフェクトのテストのためついでに追加しておく
	buf = readfile("Resources/ParticleEffectSample/ParticleEffectSample.ssbp");
	resman->regist(
		buf.data(), buf.size(),
		"effectsample",
		"Resources/ParticleEffectSample/"
	);
}

//メインループ
//Zボタンでアニメをポーズ、再開を切り替えできます。
//ポーズ中は左右キーで再生するフレームを変更できます。
static bool sstest_push = false;
static int sstest_count = 0;
static bool sstest_pause = false;
void update(float dt)
{
	char str[128];

	//パーツ名から座標等のステートの取得を行う場合はgetPartStateを使用します。
	ss::ResluteState result;
	ssplayer->getPartState(result, "body");

	//取得座用の表示
	sprintf_s(str, "body = x:%f y:%f", result.x, result.y);
	DrawString(100, 120, str, GetColor(255, 255, 255));


	//キー入力操作
	int animax = ssplayer->getMaxFrame();
	if (CheckHitKey(KEY_INPUT_ESCAPE)){
		mGameExec = 0;
	}

	if (CheckHitKey(KEY_INPUT_Z)){
		if (sstest_push == false ){
			if (sstest_pause == false ){
				sstest_pause = true;
				sstest_count = ssplayer->getCurrentFrame();;
				ssplayer->stop();
			}
			else{
				sstest_pause = false;
				ssplayer->resume();
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_UP)){
		if (sstest_push == false){
			sstest_count += 5;
			if (sstest_count >= animax){
				sstest_count = 0;
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_DOWN)){
		if (sstest_push == false){
			sstest_count -= 5;
			if (sstest_count < 0){
				sstest_count = animax - 1;
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_LEFT)){
		if (sstest_push == false){
			sstest_count--;
			if (sstest_count < 0){
				sstest_count = animax-1;
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_RIGHT)){
		if (sstest_push == false){
			sstest_count++;
			if (sstest_count >= animax){
				sstest_count = 0;
			}
		}
		sstest_push = true;
	}
	else if(CheckHitKey(KEY_INPUT_X)){
		if(sstest_push == false){
			resman->destroyPlayer(ssplayer);
			ssplayer = resman->createPlayer("character_template1", &g_eventListener);
			ssplayer->play("character_template_3head/stance");

			ssplayer->setPosition(800 / 2, 150);
			ssplayer->setScale(0.5f, 0.5f);
		}
		sstest_push = true;
	}
	else if(CheckHitKey(KEY_INPUT_C)){
		if(sstest_push == false){
			resman->destroyPlayer(ssplayer);
			ssplayer = resman->createPlayer("effectsample", &g_eventListener);
			ssplayer->play("e001/emission");

			ssplayer->setPosition(800 / 2, 300);
			ssplayer->setScale(0.5f, 0.5f);
		}
		sstest_push = true;
	}
	else{
		sstest_push = false;
	}

	if (sstest_pause == true){
		ssplayer->setCurrentFrame(sstest_count % animax);
	}
	else{
		sstest_count = ssplayer->getCurrentFrame();
	}

	//アニメーションのフレームを表示
	sprintf_s(str, "play:%d frame:%d drawCount:%d", (int)sstest_pause, sstest_count, ssplayer->getDrawSpriteCount());
	DrawString(100, 100, str, GetColor(255, 255, 255));

	//プレイヤーの更新、引数は前回の更新処理から経過した時間
	ssplayer->update(dt);

}

//描画
void draw(void)
{
	//プレイヤーの描画
	ssplayer->draw();
}

/**
* プレイヤー終了処理
*/
void relese( void )
{

	//SS5Playerの削除
	delete (ssplayer);	
	delete (resman);
}









