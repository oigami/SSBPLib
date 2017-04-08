#pragma once
#include "State.h"
#include "SS5Player.h"
#include "SS5PlayerTypes.h"
#include "math/Matrix.h"

namespace ss{
class SS5Effect;

/**
 * CustomSprite
 */
class CustomSprite{
public:
	CustomSprite();
	~CustomSprite();

	/**
	 * 行列とアルファの更新を行う。
	 * 親の情報は更新済みにしておく必要がある。
	 * 親がいない場合はrootの情報を使います。
	 */
	void updateMatrixAndAlpha(const Matrix& rootMatrix, float rootAlpha);

public:
	CustomSprite* m_parent;	//親パーツ
	State m_state;			//親子関係の計算が関係ないデータ
	Matrix m_worldMatrix;	//ワールド行列
	float m_alpha;			//[0:1]

	SSV3F_C4B_T2F_Quad m_quad;	// 頂点データ、座標、カラー値、UVが含まれる（頂点変形、サイズXY、UV移動XY、UVスケール、UV回転、反転が反映済）
	TextureID m_textureID;		// セルに対応したテクスチャ番号（ゲーム側で管理している番号を設定する）
	SSRect m_rect;				// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
	BlendType m_blendfunc;		// パーツに設定されたブレンド方法

	bool m_haveChildPlayer;	//ChildPlayer(インスタンスアニメーション)を持っているならtrue
	bool m_haveEffect;		//エフェクトを持っているならtrue
};

} //namespace ss
