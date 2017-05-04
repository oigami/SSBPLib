#include "DxLib.h"
#include "Application.h"

static const int FPS = 60;

//フレームレートの調整
void framerateControll(){
	static int previousTime;

	int updateTime = GetNowCount() - previousTime;	//1フレームにかかった時間
	int waitTime = (1000.0 / FPS) - updateTime;		//待つべき時間
	if(waitTime > 0){
		WaitTimer(waitTime);
	}
	previousTime = GetNowCount();
}

/**
* メイン関数
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif


	//DXライブラリの初期化
	ChangeWindowMode(true);	//ウインドウモード
	SetGraphMode(800, 600, GetColorBitDepth());
	if(DxLib_Init() == -1){	// ＤＸライブラリ初期化処理
		return -1;				// エラーが起きたら直ちに終了
	}
	SetDrawScreen(DX_SCREEN_BACK);


	//アプリケーション初期化
	Application application;
	application.initialize();

	//メインループ
	while(ProcessMessage() == 0 && ClearDrawScreen() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0){	//esc押しで終了
		application.update();	//ゲームの更新
		application.draw();		//ゲームの描画

		ScreenFlip();			//描画結果を画面に反映
		framerateControll();	//FPS調整
	}

	//プレイヤー終了処理
	application.finalize();


	DxLib_End();			// ＤＸライブラリ使用の終了処理
	return 0;				// ソフトの終了 
}


