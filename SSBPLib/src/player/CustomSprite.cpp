#include "CustomSprite.h"
#include "Util.h"
#include "effect/ssplayer_effect2.h"

namespace ss{

CustomSprite::CustomSprite()
	: _haveChildPlayer(false)
	, refEffect(0)
	, effectAttrInitialized(false)
{
	_state.init();
	_isStateChanged = true;
}

CustomSprite::~CustomSprite()
{
	//エフェクトクラスがある場合は解放する
	SS_SAFE_DELETE(refEffect);
}



} //namespace ss
