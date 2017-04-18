#include "State.h"
#include "DataArrayReader.h"
#include "PlayerDef.h"
#include "math/Matrix.h"
#include "InstancePartStatus.h"
#include "EffectPartStatus.h"

namespace ss{

State::State()
	: m_flags(0)
	, m_cellIndex(0)
	, m_position(0.0f, 0.0f, 0.0f)
	, m_pivot(0.0f, 0.0f)
	, m_rotation(0.0f, 0.0f, 0.0f)
	, m_scale(1.0f, 1.0f)
	, m_opacity(255)
	, m_size(1.0f, 1.0f)
	, m_uvMove(0.0f, 0.0f)
	, m_uvRotation(0.0f)
	, m_uvScale(1.0f, 1.0f)
	, m_boundingRadius(0.0f)
	, m_flipX(false)
	, m_flipY(false)
	, m_isVisibled(false)
	, m_instanceValue()
	, m_effectValue()
	, m_vertexTransform()
	, m_colorBlendVertexFunc(BLEND_MIX)
	, m_colorBlend()
{
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
	if(flags & PART_FLAG_VERTEX_TRANSFORM){		//0クリアしないので利用側はフラグを見て処理してください
		m_vertexTransform.readData(reader);
	}

	//カラーブレンド
	if(flags & PART_FLAG_COLOR_BLEND){			//0クリアしないので利用側はフラグを見て処理してください
		int typeAndFlags = reader.readU16();
		int funcNo = typeAndFlags & 0xff;
		int cb_flags = (typeAndFlags >> 8) & 0xff;

		m_colorBlendVertexFunc = static_cast<BlendType>(funcNo);
		m_colorBlend.readData(cb_flags, reader);
	}
	else{
		m_colorBlendVertexFunc = BLEND_MUL;
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
	if (m_flipX){	//左右反転を行う場合はlr入れ替え
		std::swap(q->tr.texCoords.x/*u*/, q->tl.texCoords.x/*u*/);
		std::swap(q->br.texCoords.x/*u*/, q->bl.texCoords.x/*u*/);
	}
	if (m_flipY){	//上下反転を行う場合はtb入れ替え
		std::swap(q->tr.texCoords.y/*v*/, q->br.texCoords.y/*v*/);
		std::swap(q->tl.texCoords.y/*v*/, q->bl.texCoords.y/*v*/);
	}
}


void State::vertexCompute(SSV3F_C4B_T2F_Quad* q, const SSRect& cellRect, Vector2 cellPivot) const
{
	//ひとまずcellRectに従って矩形をセットする(yは上方向+。中央(0,0)の矩形になる)
	float halfWidth = cellRect.width() * 0.5f;
	float halfHeight = cellRect.height() * 0.5f;
	q->bl.vertices = Vector3(-halfWidth, -halfHeight, 0.0f);
	q->br.vertices = Vector3( halfWidth, -halfHeight, 0.0f);
	q->tl.vertices = Vector3(-halfWidth,  halfHeight, 0.0f);
	q->tr.vertices = Vector3( halfWidth,  halfHeight, 0.0f);

	//サイズ指定があるならそちらに合わせる
	//詳しくは http://www.webtech.co.jp/help/ja/spritestudio/guide/window/main/attribute/
	//「アンカー機能を持つ特殊なパーツ」についての項目を参照
	if(m_flags & PART_FLAG_SIZE_X){
		float halfSize = m_size.x * 0.5f;
		q->bl.vertices.x = -halfSize;
		q->br.vertices.x =  halfSize;
		q->tl.vertices.x = -halfSize;
		q->tr.vertices.x =  halfSize;
	}
	if(m_flags & PART_FLAG_SIZE_Y){
		float halfSize = m_size.y * 0.5f;
		q->bl.vertices.y = -halfSize;
		q->br.vertices.y = -halfSize;
		q->tl.vertices.y =  halfSize;
		q->tr.vertices.y =  halfSize;
	}
	/* 基本の矩形作りはここまで -------------------------*/


	// 頂点変形のオフセット値を反映
	if(m_flags & PART_FLAG_VERTEX_TRANSFORM){
		q->add(m_vertexTransform);
	}


	/* セルの原点補正 -----------------------------------*/
	//セルの原点設定を反映させる
	if(m_flipX){ cellPivot.x = -cellPivot.x; }	// 水平フリップによって原点を入れ替える
	if(m_flipY){ cellPivot.y = -cellPivot.y; }	// 垂直フリップによって原点を入れ替える
	Vector2 pivot = m_pivot + cellPivot;

	//原点補正
	Vector2 cellCenter(
		(cellRect.width()  * -pivot.x),
		(cellRect.height() * +pivot.y)
	);
	q->vertexForeach([&](Vector3& vertex){
		vertex.x += cellCenter.x;		//原点補正
		vertex.y += cellCenter.y;
	});
}

void State::colorCompute(SSV3F_C4B_T2F_Quad* q) const
{
	if(m_flags & PART_FLAG_COLOR_BLEND){
		q->tl.colors = m_colorBlend.tl;
		q->tr.colors = m_colorBlend.tr;
		q->bl.colors = m_colorBlend.bl;
		q->br.colors = m_colorBlend.br;
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
