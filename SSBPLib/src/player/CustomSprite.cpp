#include "CustomSprite.h"
#include "Util.h"

namespace ss{

CustomSprite::CustomSprite() :
	 _liveFrame(0.0f)
	, refEffect(0)
	, _ssplayer(0)
	, effectAttrInitialized(false)
	, effectTimeTotal(0)
{
	_state.init();
	_isStateChanged = true;
}

CustomSprite::~CustomSprite()
{
	//エフェクトクラスがある場合は解放する
	SS_SAFE_DELETE(refEffect);
	SS_SAFE_DELETE(_ssplayer);
}


void CustomSprite::setFlippedX(bool flip)
{
	_flipX = flip;
}
void CustomSprite::setFlippedY(bool flip)
{
	_flipY = flip;
}
bool CustomSprite::isFlippedX()
{
	return (_flipX);
}
bool CustomSprite::isFlippedY()
{
	return (_flipY);
}

} //namespace ss
