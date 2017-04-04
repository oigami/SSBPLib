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
	bool				_isStateChanged;
	CustomSprite*		_parent;
	bool				_haveChildPlayer;		//ChildPlayer(インスタンスアニメーション)を持っているならtrue


	//エフェクト用パラメータ
	SS5Effect*	refEffect;

	//モーションブレンド用ステータス
	State				_orgState;

	//エフェクト制御用ワーク
	bool effectAttrInitialized;

public:
	CustomSprite();
	virtual ~CustomSprite();


	void setState(const State& state){
		_isStateChanged = true;
		_state = state;
	}
};

} //namespace ss
