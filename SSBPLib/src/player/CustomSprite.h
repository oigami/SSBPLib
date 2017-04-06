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

	bool				_haveChildPlayer;	//ChildPlayer(インスタンスアニメーション)を持っているならtrue
	bool				_haveEffect;		//エフェクトを持っているならtrue

public:
	CustomSprite();
	~CustomSprite();
};

} //namespace ss
