#include "CustomSprite.h"
#include "Util.h"
#include "effect/ssplayer_effect2.h"

namespace ss{

CustomSprite::CustomSprite()
	: m_parent(nullptr)
	, m_alpha(1.0f)
	, m_textureID(TEXTURE_ID_INVALID)
	, m_blendfunc(BLEND_MIX)
	, m_partType(PARTTYPE_INVALID)
{
}

CustomSprite::~CustomSprite()
{
}


//Stateの内容を元に情報更新する
void CustomSprite::update(const CellRef* cellRef)
{
	//セルの原点設定を反映させる
	Vector2 pivot = m_state.m_pivot;
	if(cellRef){
		float cpx = cellRef->m_pivot.x;
		float cpy = cellRef->m_pivot.y;

		if(m_state.m_flipX){ cpx = -cpx; }	// 水平フリップによって原点を入れ替える
		if(m_state.m_flipY){ cpy = -cpy; }	// 垂直フリップによって原点を入れ替える

		pivot.x += cpx;
		pivot.y += cpy;
	}



	//頂点データの設定
	//quadにはプリミティブの座標（頂点変形を含む）、UV、カラー値が設定されます。
	SSV3F_C4B_T2F_Quad quad;
	SSRect cellRect;
	if(cellRef){
		cellRect = cellRef->m_rect;
	}
	m_state.vertexCompute(&quad, cellRect);

	//原点補正
	Vector3 center(
		(cellRect.width() * -(pivot.x)),
		(cellRect.height() * +(pivot.y)),
		0.0f
	);
	quad.vertexForeach([&](Vector3& vertex){
		vertex += center;		//原点補正
	});


	//カラーの設定
	m_state.colorCompute(&quad);


	//UVを設定する
	SSTex2F uv_tl, uv_br;
	if(cellRef){
		uv_tl = cellRef->m_uv1;
		uv_br = cellRef->m_uv2;
	}
	m_state.uvCompute(&quad, uv_tl, uv_br);

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


//draw呼ぶか?
bool CustomSprite::isVisibled() const{
	if(m_textureID == TEXTURE_ID_INVALID){
		//セルが無く通常パーツ、ヌルパーツの時は非表示にする
		if((m_partType == PARTTYPE_NORMAL) || (m_partType == PARTTYPE_NULL)){
			return false;
		}
	}
	return m_state.m_isVisibled;
}


bool CustomSprite::isInstancePart() const{
	return (m_partType == PARTTYPE_INSTANCE);
}
bool CustomSprite::isEffectPart() const{
	return (m_partType == PARTTYPE_EFFECT);
}


} //namespace ss
