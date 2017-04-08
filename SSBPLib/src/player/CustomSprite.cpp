#include "CustomSprite.h"
#include "Util.h"
#include "effect/ssplayer_effect2.h"

namespace ss{

CustomSprite::CustomSprite()
	: m_parent(nullptr)
	, m_alpha(1.0f)
	, m_textureID(TEXTURE_ID_INVALID)
	, m_blendfunc(BLEND_MIX)
	, m_haveChildPlayer(false)
	, m_haveEffect(false)
{
}

CustomSprite::~CustomSprite()
{
	//Load,Releaseのイベントは外でやるため、それに対応してtrue,falseを書き換えるようにしてください
	SS_ASSERT(m_haveChildPlayer == false);
	SS_ASSERT(m_haveEffect == false);
}



} //namespace ss
