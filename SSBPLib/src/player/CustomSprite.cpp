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
}

CustomSprite::~CustomSprite()
{
	//エフェクトクラスがある場合は解放する
	SS_SAFE_DELETE(refEffect);
	SS_SAFE_DELETE(_ssplayer);
}


CustomSprite* CustomSprite::create()
{
	CustomSprite *pSprite = new CustomSprite();
	if(pSprite){
		pSprite->initState();
		return pSprite;
	}
	SS_SAFE_DELETE(pSprite);
	return NULL;
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
