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
	Matrix				_mat;		//ワールド行列
	State				_state;		//親子関係の計算が関係ないデータ（・・・に、したい。）
	CustomSprite*		_parent;
	float				_alpha;		//[0:1]

	SSV3F_C4B_T2F_Quad m_quad;	/// 頂点データ、座標、カラー値、UVが含まれる（頂点変形、サイズXY、UV移動XY、UVスケール、UV回転、反転が反映済）
	TextureID m_textureID;		/// セルに対応したテクスチャ番号（ゲーム側で管理している番号を設定する）
	SSRect m_rect;				/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
	BlendType m_blendfunc;		/// パーツに設定されたブレンド方法

	bool				_haveChildPlayer;	//ChildPlayer(インスタンスアニメーション)を持っているならtrue
	bool				_haveEffect;		//エフェクトを持っているならtrue

public:
	CustomSprite();
	~CustomSprite();
};

} //namespace ss
