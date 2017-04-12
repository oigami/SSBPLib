#pragma once

#include "SS5PlayerTypes.h"
#include <memory>
#include "math/SSRect.h"
#include "math/Matrix.h"
#include "InstancePartStatus.h"
#include "EffectPartStatus.h"
#include "PlayerDef.h"

namespace ss{
class DataArrayReader;
struct AnimationInitialData;


/**
 * State
 * パーツの情報を格納します。Stateの内容をもとに描画処理を作成してください。
 */
struct State{
	int m_flags;					/// このフレームで更新が行われるステータスのフラグ
	int m_cellIndex;				/// パーツに割り当てられたセルの番号
	Vector3 m_position;				/// SS5アトリビュート：座標(XYZ)
	Vector2 m_pivot;				/// 原点Xオフセット＋セルに設定された原点オフセットX, 原点Yオフセット＋セルに設定された原点オフセットY
	Vector3 m_rotation;				/// 回転(XYZ)
	Vector2 m_scale;				/// スケール(X,Y)
	int     m_opacity;				/// 不透明度（0～255）
	Vector2 m_size;					/// SS5アトリビュート：サイズ(X,Y)
	Vector2 m_uvMove;				/// SS5アトリビュート：UV 移動(X,Y)
	float   m_uvRotation;			/// SS5アトリビュート：UV 回転
	Vector2 m_uvScale;				/// SS5アトリビュート：UV スケール(X,Y)
	float   m_boundingRadius;		/// SS5アトリビュート：当たり半径
	
	bool m_flipX;					/// 横反転
	bool m_flipY;					/// 縦反転
	bool m_isVisibled;				/// 非表示

	BlendType m_colorBlendVertexFunc;	/// SS5アトリビュート：カラーブレンドのブレンド方法
	int m_colorBlendVertexFlags;		/// SS5アトリビュート：カラーブレンドの単色か頂点カラーか。

	
	InstancePartStatus m_instanceValue;	//インスタンスアトリビュート
	EffectPartStatus m_effectValue;		//エフェクトアトリビュート

	SSQuad3 m_vertexTransform;	//頂点変形のオフセット


	//readerを介してデータを読み取る
	void readData(DataArrayReader& reader, const AnimationInitialData* init);



	//現在のStateの情報を元にuvを計算する
	void uvCompute(SSV3F_C4B_T2F_Quad* q, SSTex2F uv_tl/*top,left*/, SSTex2F uv_br/*bottom,right*/) const;
	//現在のStateの情報を元にverexを計算する
	void vertexCompute(SSV3F_C4B_T2F_Quad* q, const SSRect& cellRect/*, const SSQuad3& vertexTransform*/) const;

	//現在のStateの情報を元にローカルmatrixを計算する
	Matrix getLocalMatrix() const;
	
	//アルファ値を取得[0:1]
	float getAlpha() const;


	void init();

	//bool operator==(const State &s) const;
	//bool operator!=(const State &s) const;

	State() { init(); }
};

} //namespace ss
