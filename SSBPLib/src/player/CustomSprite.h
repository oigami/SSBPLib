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
	Matrix				_mat;
	State				_state;
	CustomSprite*		_parent;
	bool				_haveChildPlayer;		//ChildPlayer(インスタンスアニメーション)を持っているならtrue


	//エフェクト用パラメータ
	SS5Effect*	refEffect;

	//モーションブレンド用ステータス
	State				_orgState;

public:
	CustomSprite();
	~CustomSprite();
};

} //namespace ss
