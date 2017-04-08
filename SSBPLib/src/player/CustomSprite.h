#pragma once
#include "State.h"
#include "SS5Player.h"
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

	bool				_haveChildPlayer;	//ChildPlayer(インスタンスアニメーション)を持っているならtrue
	bool				_haveEffect;		//エフェクトを持っているならtrue

public:
	CustomSprite();
	~CustomSprite();
};

} //namespace ss
