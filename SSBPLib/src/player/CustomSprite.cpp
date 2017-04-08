#include "CustomSprite.h"
#include "Util.h"
#include "effect/ssplayer_effect2.h"

namespace ss{

CustomSprite::CustomSprite()
	: _alpha(1.0f)
	, m_textureID(TEXTURE_ID_INVALID)
	, m_blendfunc(BLEND_MIX)
	, _haveChildPlayer(false)
	, _haveEffect(false)
{
	_state.init();
}

CustomSprite::~CustomSprite()
{
	//Load,Releaseのイベントは外でやるため、それに対応してtrue,falseを書き換えるようにしてください
	SS_ASSERT(_haveChildPlayer == false);
	SS_ASSERT(_haveEffect == false);
}



} //namespace ss
