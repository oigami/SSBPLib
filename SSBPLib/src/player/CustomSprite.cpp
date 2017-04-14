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


//Stateの内容を元に情報更新する
void CustomSprite::update(const CellRef* cellRef, const State& state)
{
	//頂点データの設定
	//quadにはプリミティブの座標（頂点変形を含む）、UV、カラー値が設定されます。
	SSV3F_C4B_T2F_Quad quad;
	SSRect cellRect;
	if(cellRef){
		cellRect = cellRef->m_rect;
	}
	state.vertexCompute(&quad, cellRect);

	//原点補正
	Vector3 center(
		(m_rect.width() * -(state.m_pivot.x)),
		(m_rect.height() * +(state.m_pivot.y)),	//xと同様、-のような気がする
		0.0f
	);
	quad.vertexForeach([&](Vector3& vertex){
		vertex += center;		//原点補正
	});


	//カラーの設定
	state.colorCompute(&quad);


	//UVを設定する
	SSTex2F uv_tl, uv_br;
	if(cellRef){
		uv_tl = cellRef->m_uv1;
		uv_br = cellRef->m_uv2;
	}
	state.uvCompute(&quad, uv_tl, uv_br);

	m_quad = quad;
}


void CustomSprite::updateMatrixAndAlpha(const Matrix& rootMatrix, float rootAlpha)
{
	//親から結果を伝播させる。親がない場合はrootから。
	if(m_parent == nullptr){
		m_worldMatrix = m_state.getLocalMatrix() * rootMatrix;
		m_alpha = m_state.getAlpha() * rootAlpha;
	}
	else{
		m_worldMatrix = m_state.getLocalMatrix() * m_parent->m_worldMatrix;
		m_alpha = m_state.getAlpha() * m_parent->m_alpha;
	}
}


} //namespace ss
