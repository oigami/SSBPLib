#include "State.h"
#include "DataArrayReader.h"
#include "PlayerDef.h"
#include "math/Matrix.h"
#include "InstancePartStatus.h"
#include "EffectPartStatus.h"

namespace ss{

void State::init()
{
	m_flags      = 0;
	m_cellIndex  = 0;
	m_position   = Vector3::zero;
	m_pivot      = Vector2::zero;
	m_rotation   = Vector3::zero;
	m_scale      = Vector2::one;
	m_opacity    = 255;
	m_size       = Vector2::one;
	m_uvMove     = Vector2::zero;
	m_uvRotation = 0.0f;
	m_uvScale    = Vector2::one;
	m_boundingRadius = 0.0f;

	m_flipX = false;
	m_flipY = false;
	m_isVisibled = false;

	m_instanceValue = InstancePartStatus();
	m_effectValue = EffectPartStatus();

	m_vertexTransform = SSQuad3();

	m_colorBlendVertexFunc = BLEND_MIX;
	m_colorBlendVertexFlags = 0;
	m_colorTL = SSColor4B(0xff, 0xff, 0xff, 0xff);
	m_colorTR = SSColor4B(0xff, 0xff, 0xff, 0xff);
	m_colorBL = SSColor4B(0xff, 0xff, 0xff, 0xff);
	m_colorBR = SSColor4B(0xff, 0xff, 0xff, 0xff);
}


void State::readData(DataArrayReader& reader, const AnimationInitialData* init)
{
	// optional parameters
	int flags = reader.readU32();
	m_flags      = flags;
	m_cellIndex  = flags & PART_FLAG_CELL_INDEX ? reader.readS16() : init->cellIndex;
	m_position.x = flags & PART_FLAG_POSITION_X ? reader.readFloat() : init->positionX;
	m_position.y = flags & PART_FLAG_POSITION_Y ? reader.readFloat() : init->positionY;
	m_position.z = flags & PART_FLAG_POSITION_Z ? reader.readFloat() : init->positionZ;
	m_pivot.x    = flags & PART_FLAG_PIVOT_X ? reader.readFloat() : init->pivotX;
	m_pivot.y    = flags & PART_FLAG_PIVOT_Y ? reader.readFloat() : init->pivotY;
	m_rotation.x = flags & PART_FLAG_ROTATIONX ? reader.readFloat() : init->rotationX;
	m_rotation.y = flags & PART_FLAG_ROTATIONY ? reader.readFloat() : init->rotationY;
	m_rotation.z = flags & PART_FLAG_ROTATIONZ ? reader.readFloat() : init->rotationZ;
	m_scale.x    = flags & PART_FLAG_SCALE_X ? reader.readFloat() : init->scaleX;
	m_scale.y    = flags & PART_FLAG_SCALE_Y ? reader.readFloat() : init->scaleY;
	m_opacity    = flags & PART_FLAG_OPACITY ? reader.readU16() : init->opacity;
	m_size.x     = flags & PART_FLAG_SIZE_X ? reader.readFloat() : init->size_X;
	m_size.y     = flags & PART_FLAG_SIZE_Y ? reader.readFloat() : init->size_Y;
	m_uvMove.x   = flags & PART_FLAG_U_MOVE ? reader.readFloat() : init->uv_move_X;
	m_uvMove.y   = flags & PART_FLAG_V_MOVE ? reader.readFloat() : init->uv_move_Y;
	m_uvRotation = flags & PART_FLAG_UV_ROTATION ? reader.readFloat() : init->uv_rotation;
	m_uvScale.x  = flags & PART_FLAG_U_SCALE ? reader.readFloat() : init->uv_scale_X;
	m_uvScale.y  = flags & PART_FLAG_V_SCALE ? reader.readFloat() : init->uv_scale_Y;
	m_boundingRadius = flags & PART_FLAG_BOUNDINGRADIUS ? reader.readFloat() : init->boundingRadius;

	m_instanceValue.readData(flags, reader, init);	//インスタンスアトリビュート
	m_effectValue.readData(flags, reader, init);	//エフェクトアトリビュート

	//フラグを抽出
	m_flipX = (flags & PART_FLAG_FLIP_H);
	m_flipY = (flags & PART_FLAG_FLIP_V);
	m_isVisibled = !(flags & PART_FLAG_INVISIBLE);

	//頂点変形のオフセット
	if(flags & PART_FLAG_VERTEX_TRANSFORM){
		m_vertexTransform.readData(reader);
	}

	//カラーブレンド
	m_colorTL = SSColor4B(0xff, 0xff, 0xff, 0xff);
	m_colorTR = SSColor4B(0xff, 0xff, 0xff, 0xff);
	m_colorBL = SSColor4B(0xff, 0xff, 0xff, 0xff);
	m_colorBR = SSColor4B(0xff, 0xff, 0xff, 0xff);
	if(flags & PART_FLAG_COLOR_BLEND){
		int typeAndFlags = reader.readU16();
		int funcNo = typeAndFlags & 0xff;
		int cb_flags = (typeAndFlags >> 8) & 0xff;

		m_colorBlendVertexFunc = static_cast<BlendType>(funcNo);
		m_colorBlendVertexFlags = cb_flags;

		//ssbpではカラーブレンドのレート（％）は使用できません。
		//制限となります。
		if(cb_flags & VERTEX_FLAG_ONE){
			SSColor4B color;
			color.readColorWithRate(reader);
			
			m_colorTL = m_colorTR = m_colorBL = m_colorBR = color;
		}
		else{
			if(cb_flags & VERTEX_FLAG_LT){
				m_colorTL.readColorWithRate(reader);
			}
			if(cb_flags & VERTEX_FLAG_RT){
				m_colorTR.readColorWithRate(reader);
			}
			if(cb_flags & VERTEX_FLAG_LB){
				m_colorBL.readColorWithRate(reader);
			}
			if(cb_flags & VERTEX_FLAG_RB){
				m_colorBR.readColorWithRate(reader);
			}
		}
	}
}


void State::uvCompute(SSV3F_C4B_T2F_Quad *q, SSTex2F uv_tl, SSTex2F uv_br) const
{
	/* MEMO: AnimationInitialDataの値について。
	 * 通常は最初のフレームの値がセットされているが、
	 * uvに関しては、値は次のようになっている
	 * SS5SDK\Build\Converter\main.cpp
	 * static Lump* parseParts(SsProject* proj, const std::string& imageBaseDir)
	 *     ...
	 * 			init.uv_move_X = 0;
	 *			init.uv_move_Y = 0;
	 *			init.uv_rotation = 0;
	 *			init.uv_scale_X = 1;
	 *			init.uv_scale_Y = 1;
	 */

	q->tl.texCoords = uv_tl;
	q->tr.texCoords = SSTex2F(uv_br.u(), uv_tl.v());
	q->bl.texCoords = SSTex2F(uv_tl.u(), uv_br.v());
	q->br.texCoords = uv_br;

	//uvスクロール
	if(m_flags & (PART_FLAG_U_MOVE | PART_FLAG_V_MOVE)){
		q->tl.texCoords += m_uvMove;
		q->tr.texCoords += m_uvMove;
		q->bl.texCoords += m_uvMove;
		q->br.texCoords += m_uvMove;
	}
	

	//計算用にuv中心を求めておく
	//memo:回転・スケールで中央への戻し処理が多発するのでボトルネックになるようなら最初から中心合わせで計算して最後にオフセットを足すようにするのがいいかもしれない
	SSTex2F uvCenter = (q->br.texCoords + q->tl.texCoords) / 2.0f;
	
	//UV回転
	if (m_flags & PART_FLAG_UV_ROTATION){
		//頂点位置を回転させる
		float rotateRadian = SSDegToRad(m_uvRotation);
		q->tl.texCoords.rotate(rotateRadian, uvCenter);
		q->tr.texCoords.rotate(rotateRadian, uvCenter);
		q->bl.texCoords.rotate(rotateRadian, uvCenter);
		q->br.texCoords.rotate(rotateRadian, uvCenter);
	}

	//UVスケール
	if(m_flags & (PART_FLAG_U_SCALE | PART_FLAG_V_SCALE)){
		q->uvForeach([&](SSTex2F &uv){
			//中心を基準として拡大縮小させる
			uv -= uvCenter;
			uv.x/*u*/ *= m_uvScale.x;
			uv.y/*v*/ *= m_uvScale.y;
			uv += uvCenter;
		});
	}

	//UV反転
	if (m_flags & PART_FLAG_FLIP_H){	//左右反転を行う場合はlr入れ替え
		std::swap(q->tr.texCoords.x/*u*/, q->tl.texCoords.x/*u*/);
		std::swap(q->br.texCoords.x/*u*/, q->bl.texCoords.x/*u*/);
	}
	if (m_flags & PART_FLAG_FLIP_H){	//上下反転を行う場合はtb入れ替え
		std::swap(q->tr.texCoords.y/*v*/, q->br.texCoords.y/*v*/);
		std::swap(q->tl.texCoords.y/*v*/, q->bl.texCoords.y/*v*/);
	}
}


void State::vertexCompute(SSV3F_C4B_T2F_Quad* q, const SSRect& cellRect/*, const SSQuad3& vertexTransform*/) const
{
	//ひとまずrectをベースに矩形をセットする
	float width = cellRect.width();
	float height = cellRect.height();
	q->bl.vertices = Vector3(0, 0, 0);
	q->br.vertices = Vector3(width, 0, 0);
	q->tl.vertices = Vector3(0, height, 0);
	q->tr.vertices = Vector3(width, height, 0);  //yが上方向+なので、左下基準(0,0)の矩形になる

	//サイズ指定があるならそちらに合わせる
	//詳しくは http://www.webtech.co.jp/help/ja/spritestudio/guide/window/main/attribute/
	//「アンカー機能を持つ特殊なパーツ」についての項目を参照
	if(m_flags & PART_FLAG_SIZE_X){
		q->br.vertices.x = m_size.x;
		q->tr.vertices.x = m_size.x;
	}
	if(m_flags & PART_FLAG_SIZE_Y){
		q->tl.vertices.y = m_size.y;
		q->tr.vertices.y = m_size.y;
	}

	//中心が(0,0)になるようにオフセットを追加
	Vector3 center = (q->bl.vertices + q->tr.vertices) / 2;
	q->bl.vertices -= center;
	q->br.vertices -= center;
	q->tl.vertices -= center;
	q->tr.vertices -= center;


	// 頂点変形のオフセット値を反映
	if(m_flags & PART_FLAG_VERTEX_TRANSFORM){
		q->add(m_vertexTransform);
	}
}

void State::colorCompute(SSV3F_C4B_T2F_Quad* q) const
{
	if(m_flags & PART_FLAG_COLOR_BLEND){
		q->tl.colors = m_colorTL;
		q->tr.colors = m_colorTR;
		q->bl.colors = m_colorBL;
		q->br.colors = m_colorBR;
	}
	else{
		q->tl.colors = SSColor4B(0xff, 0xff, 0xff, 0xff);
		q->tr.colors = SSColor4B(0xff, 0xff, 0xff, 0xff);
		q->bl.colors = SSColor4B(0xff, 0xff, 0xff, 0xff);
		q->br.colors = SSColor4B(0xff, 0xff, 0xff, 0xff);
	}
}



// SRzRyRxT mat
Matrix State::getLocalMatrix() const
{
	Matrix mat;
	mat.setupSRzyxT(
		Vector3(m_scale.x, m_scale.y, 1.0f),
		SSDegToRad(m_rotation),
		m_position
	);
	return mat;
}

//alpha
float State::getAlpha() const
{
	return m_opacity / 255.0f;	//[0:1]に変換
}


} //namespace ss
