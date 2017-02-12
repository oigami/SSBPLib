#pragma once
#include "State.h"
#include "SS5Player.h"
#include "math/Matrix.h"

namespace ss{

/**
 * CustomSprite
 */
class CustomSprite{
private:
	bool				_flipX;
	bool				_flipY;

public:
	Matrix				_mat;
	State				_state;
	bool				_isStateChanged;
	CustomSprite*		_parent;
	Player*				_ssplayer;
	float				_liveFrame;

	//エフェクト用パラメータ
	SsEffectRenderV2*	refEffect;
	SsPartState			partState;

	//モーションブレンド用ステータス
	State				_orgState;

	//エフェクト制御用ワーク
	bool effectAttrInitialized;
	float effectTimeTotal;

public:
	CustomSprite();
	virtual ~CustomSprite();

	void setStateValue(float& ref, float value)
	{
		if(ref != value)
		{
			ref = value;
			_isStateChanged = true;
		}
	}

	void setState(const State& state){
		_isStateChanged = true;
		_state = state;
	}


	void setFlippedX(bool flip);
	void setFlippedY(bool flip);
	bool isFlippedX();
	bool isFlippedY();
};

} //namespace ss
