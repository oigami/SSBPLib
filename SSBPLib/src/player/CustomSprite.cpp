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
	Matrix mat;

	if(m_parent == nullptr){
		//rootパーツはプレイヤーからステータスを引き継ぐ
		mat = rootMatrix;
	}
	else{
		//親のマトリクスを適用
		mat = m_parent->m_worldMatrix;
	}
	mat = m_state.getLocalMatrix() * mat;

	m_worldMatrix = mat;

	if(m_parent == nullptr){	//root.
		m_alpha = m_state.m_opacity / 255.0f;
		m_alpha *= rootAlpha;
	}
	else {
		//アルファは親の影響を受ける
		m_alpha = m_state.m_opacity / 255.0f;
		m_alpha *= m_parent->m_alpha;
	}

}


} //namespace ss
