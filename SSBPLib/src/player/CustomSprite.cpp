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


void CustomSprite::updateMatrixAndAlpha(const Matrix& rootMatrix, float rootAlpha)
{
	//親から結果を伝播させる。親がない場合はrootから。
	if(m_parent == nullptr){
		m_worldMatrix = m_state.getLocalMatrix() * rootMatrix;
		m_alpha = m_state.m_opacity / 255.0f;
		m_alpha *= rootAlpha;
	}
	else{
		m_worldMatrix = m_state.getLocalMatrix() * m_parent->m_worldMatrix;
		m_alpha = m_state.m_opacity / 255.0f;
		m_alpha *= m_parent->m_alpha;
	}
}


} //namespace ss
