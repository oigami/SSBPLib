#pragma once

#include "SS5PlayerTypes.h"
#include <memory>
#include "math/SSRect.h"

namespace ss{
class DataArrayReader;
struct AnimationInitialData;


/**
 * State
 * パーツの情報を格納します。Stateの内容をもとに描画処理を作成してください。
 */
struct State{
	int flags;						/// このフレームで更新が行われるステータスのフラグ
	int cellIndex;					/// パーツに割り当てられたセルの番号
	float x;						/// SS5アトリビュート：X座標
	float y;						/// SS5アトリビュート：Y座標
	float z;						/// SS5アトリビュート：Z座標
	float pivotX;					/// 原点Xオフセット＋セルに設定された原点オフセットX
	float pivotY;					/// 原点Yオフセット＋セルに設定された原点オフセットY
	float rotationX;				/// X回転
	float rotationY;				/// Y回転
	float rotationZ;				/// Z回転
	float scaleX;					/// Xスケール
	float scaleY;					/// Yスケール
	int opacity;					/// 不透明度（0～255）
	float size_X;					/// SS5アトリビュート：Xサイズ
	float size_Y;					/// SS5アトリビュート：Xサイズ
	float uv_move_X;				/// SS5アトリビュート：UV X移動
	float uv_move_Y;				/// SS5アトリビュート：UV Y移動
	float uv_rotation;				/// SS5アトリビュート：UV 回転
	float uv_scale_X;				/// SS5アトリビュート：UV Xスケール
	float uv_scale_Y;				/// SS5アトリビュート：UV Yスケール
	float boundingRadius;			/// SS5アトリビュート：当たり半径
	
	bool flipX;						/// 横反転（親子関係計算済）
	bool flipY;						/// 縦反転（親子関係計算済）
	bool isVisibled;				/// 非表示（親子関係計算済）


	int colorBlendFunc;				/// SS5アトリビュート：カラーブレンドのブレンド方法
	int colorBlendType;				/// SS5アトリビュート：カラーブレンドの単色か頂点カラーか。
	SSV3F_C4B_T2F_Quad quad;		/// 頂点データ、座標、カラー値、UVが含まれる（頂点変形、サイズXY、UV移動XY、UVスケール、UV回転、反転が反映済）
	TextuerData texture;			/// セルに対応したテクスチャ番号（ゲーム側で管理している番号を設定する）
	SSRect rect;					/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
	int blendfunc;					/// パーツに設定されたブレンド方法
	float mat[16];					/// パーツの位置を算出するためのマトリクス（親子関係計算済）
																
	//再生用パラメータ
	float Calc_rotationX;			/// X回転（親子関係計算済）
	float Calc_rotationY;			/// Y回転（親子関係計算済）
	float Calc_rotationZ;			/// Z回転（親子関係計算済）
	float Calc_scaleX;				/// Xスケール（親子関係計算済）
	float Calc_scaleY;				/// Yスケール（親子関係計算済）
	int Calc_opacity;				/// 不透明度（0～255）（親子関係計算済）
	//インスタンスアトリビュート
	int			instanceValue_curKeyframe;
	int			instanceValue_startFrame;
	int			instanceValue_endFrame;
	int			instanceValue_loopNum;
	float		instanceValue_speed;
	int			instanceValue_loopflag;
	//エフェクトアトリビュート
	int			effectValue_curKeyframe;
	int			effectValue_startTime;
	float		effectValue_speed;
	int			effectValue_loopflag;


	//readerを介してデータを読み取る
	void readData(DataArrayReader& reader, const AnimationInitialData* init);


	////現在のStateの情報を元にuvを計算する
	//void uvCompute(SSV3F_C4B_T2F_Quad *q, const SSRect &cellRect, int imgWidth, int imgHeight) const;
	////現在のStateの情報を元にverexを計算する
	//void vertexCompute(SSV3F_C4B_T2F_Quad *q, const SSRect &cellRect, const SSQuad3 &vertexTransform) const;
	////現在のStateの情報を元にmatrixを計算する。setIdentityから始めます
	//void matrixCompute(Matrix *matrix) const;

	void init();

	//bool operator==(const State &s) const;
	//bool operator!=(const State &s) const;

	State() { init(); }
};

} //namespace ss
