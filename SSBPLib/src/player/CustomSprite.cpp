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
	if(!cellRef){
		//セルがないとき
		m_state.vertexCompute(&m_quad, SSRect(0.0f, 0.0f, 0.0f, 0.0f));	//頂点
		m_state.colorCompute(&m_quad);									//カラー
		m_state.uvCompute(&m_quad, SSTex2F::zero, SSTex2F::zero);		//UV
		return;
	}

	//quadにはローカル座標系での、頂点座標(頂点変形を含む)、UV、カラー値が設定されます
	const SSRect& cellRect = cellRef->m_rect;
	m_state.vertexCompute(&m_quad, cellRect);								//頂点
	m_state.colorCompute(&m_quad);											//カラー
	m_state.uvCompute(&m_quad, cellRef->m_uv1/*tl*/, cellRef->m_uv2/*br*/);	//UV


	//セルの原点設定を反映させる
	Vector2 pivot = m_state.m_pivot;
	{
		Vector2 cellPivot = cellRef->m_pivot;

		if(m_state.m_flipX){ cellPivot.x = -cellPivot.x; }	// 水平フリップによって原点を入れ替える
		if(m_state.m_flipY){ cellPivot.y = -cellPivot.y; }	// 垂直フリップによって原点を入れ替える
		
		pivot += cellPivot;
	}

	//原点補正
	Vector2 center(
		(cellRect.width() * -(pivot.x)),
		(cellRect.height() * +(pivot.y))
	);
	m_quad.vertexForeach([&](Vector3& vertex){
		vertex.x += center.x;		//原点補正
		vertex.y += center.y;
	});
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
