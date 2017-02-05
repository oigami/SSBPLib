#include "State.h"
#include "DataArrayReader.h"
#include "PlayerDef.h"

namespace ss{

void State::init()
{
	flags = 0;
	cellIndex = 0;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	pivotX = 0.0f;
	pivotY = 0.0f;
	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;
	scaleX = 1.0f;
	scaleY = 1.0f;
	opacity = 255;
	size_X = 1.0f;
	size_Y = 1.0f;
	uv_move_X = 0.0f;
	uv_move_Y = 0.0f;
	uv_rotation = 0.0f;
	uv_scale_X = 1.0f;
	uv_scale_Y = 1.0f;
	boundingRadius = 0.0f;
	flipX = false;
	flipY = false;
	isVisibled = false;

	colorBlendFunc = 0;
	colorBlendType = 0;
	memset(&quad, 0, sizeof(quad));
	texture.handle = 0;
	texture.size_w = 0;
	texture.size_h = 0;
	rect = SSRect(0, 0, 0, 0);
	blendfunc = 0;
	memset(&mat, 0, sizeof(mat));

	instanceValue_curKeyframe = 0;
	instanceValue_startFrame = 0;
	instanceValue_endFrame = 0;
	instanceValue_loopNum = 0;
	instanceValue_speed = 0;
	instanceValue_loopflag = 0;
	effectValue_curKeyframe = 0;
	effectValue_startTime = 0;
	effectValue_speed = 0;
	effectValue_loopflag = 0;

	Calc_rotationX = 0.0f;
	Calc_rotationY = 0.0f;
	Calc_rotationZ = 0.0f;
	Calc_scaleX = 1.0f;
	Calc_scaleY = 1.0f;
	Calc_opacity = 255;

}


void State::readData(DataArrayReader& reader, const AnimationInitialData* init)
{
	// optional parameters
	flags      = reader.readU32();
	cellIndex  = flags & PART_FLAG_CELL_INDEX ? reader.readS16() : init->cellIndex;
	x        = flags & PART_FLAG_POSITION_X ? reader.readFloat() : init->positionX;
#ifdef UP_MINUS
	y        = flags & PART_FLAG_POSITION_Y ? -reader.readFloat() : -init->positionY;		//上がマイナスなので反転させる
#else
	y        = flags & PART_FLAG_POSITION_Y ? reader.readFloat() : init->positionY;
#endif
	z        = flags & PART_FLAG_POSITION_Z ? reader.readFloat() : init->positionZ;
	pivotX   = flags & PART_FLAG_PIVOT_X ? reader.readFloat() : init->pivotX;
#ifdef UP_MINUS
	pivotY = flags & PART_FLAG_PIVOT_Y ? -reader.readFloat() : -init->pivotY;
#else
	pivotY = flags & PART_FLAG_PIVOT_Y ? reader.readFloat() : init->pivotY;
#endif
#ifdef UP_MINUS
	rotationX = flags & PART_FLAG_ROTATIONX ? -reader.readFloat() : -init->rotationX;
	rotationY = flags & PART_FLAG_ROTATIONY ? -reader.readFloat() : -init->rotationY;
	rotationZ = flags & PART_FLAG_ROTATIONZ ? -reader.readFloat() : -init->rotationZ;
#else
	rotationX = flags & PART_FLAG_ROTATIONX ? reader.readFloat() : init->rotationX;
	rotationY = flags & PART_FLAG_ROTATIONY ? reader.readFloat() : init->rotationY;
	rotationZ = flags & PART_FLAG_ROTATIONZ ? reader.readFloat() : init->rotationZ;
#endif
	scaleX = flags & PART_FLAG_SCALE_X ? reader.readFloat() : init->scaleX;
	scaleY   = flags & PART_FLAG_SCALE_Y ? reader.readFloat() : init->scaleY;
	opacity    = flags & PART_FLAG_OPACITY ? reader.readU16() : init->opacity;
	size_X   = flags & PART_FLAG_SIZE_X ? reader.readFloat() : init->size_X;
	size_Y   = flags & PART_FLAG_SIZE_Y ? reader.readFloat() : init->size_Y;
	uv_move_X   = flags & PART_FLAG_U_MOVE ? reader.readFloat() : init->uv_move_X;
	uv_move_Y   = flags & PART_FLAG_V_MOVE ? reader.readFloat() : init->uv_move_Y;
	uv_rotation = flags & PART_FLAG_UV_ROTATION ? reader.readFloat() : init->uv_rotation;
	uv_scale_X  = flags & PART_FLAG_U_SCALE ? reader.readFloat() : init->uv_scale_X;
	uv_scale_Y  = flags & PART_FLAG_V_SCALE ? reader.readFloat() : init->uv_scale_Y;
	boundingRadius = flags & PART_FLAG_BOUNDINGRADIUS ? reader.readFloat() : init->boundingRadius;

	//インスタンスアトリビュート
	instanceValue_curKeyframe	= flags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_curKeyframe;
	instanceValue_startFrame	= flags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_startFrame;
	instanceValue_endFrame		= flags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_endFrame;
	instanceValue_loopNum		= flags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_loopNum;
	instanceValue_speed			= flags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readFloat() : init->instanceValue_speed;
	instanceValue_loopflag		= flags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_loopflag;
	//エフェクトアトリビュート
	effectValue_curKeyframe		= flags & PART_FLAG_EFFECT_KEYFRAME ? reader.readS32() : init->effectValue_curKeyframe;
	effectValue_startTime		= flags & PART_FLAG_EFFECT_KEYFRAME ? reader.readS32() : init->effectValue_startTime;
	effectValue_speed			= flags & PART_FLAG_EFFECT_KEYFRAME ? reader.readFloat() : init->effectValue_speed;
	effectValue_loopflag		= flags & PART_FLAG_EFFECT_KEYFRAME ? reader.readS32() : init->effectValue_loopflag;


	flipX = (flags & PART_FLAG_FLIP_H);
	flipY = (flags & PART_FLAG_FLIP_V);

	isVisibled = !(flags & PART_FLAG_INVISIBLE);
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
	if(flags & (PART_FLAG_U_MOVE | PART_FLAG_V_MOVE)){
		SSTex2F uvMove(this->uv_move_X, this->uv_move_Y);
		q->tl.texCoords += uvMove;
		q->tr.texCoords += uvMove;
		q->bl.texCoords += uvMove;
		q->br.texCoords += uvMove;
	}
	

	float u_wide = 0;
	float v_height = 0;
	float u_center = 0;
	float v_center = 0;

	//UVを作成、反転の結果UVが反転する
	u_wide = (q->tr.texCoords.u() - q->tl.texCoords.u()) / 2.0f;
	v_height = (q->bl.texCoords.v() - q->tl.texCoords.v()) / 2.0f;
	u_center = q->tl.texCoords.u() + u_wide;
	v_center = q->tl.texCoords.v() + v_height;
	
	//UV回転
	if (this->flags & PART_FLAG_UV_ROTATION){
		//頂点位置を回転させる
		q->tl.texCoords.rotate(SSDegToRad(this->uv_rotation), Vector2(u_center, v_center));
		q->tr.texCoords.rotate(SSDegToRad(this->uv_rotation), Vector2(u_center, v_center));
		q->bl.texCoords.rotate(SSDegToRad(this->uv_rotation), Vector2(u_center, v_center));
		q->br.texCoords.rotate(SSDegToRad(this->uv_rotation), Vector2(u_center, v_center));
	}

	//UVスケール || 反転
	if (this->flags & PART_FLAG_U_SCALE){
		q->tl.texCoords.x/*u*/ = u_center - (u_wide * this->uv_scale_X);
		q->tr.texCoords.x/*u*/ = u_center + (u_wide * this->uv_scale_X);
		q->bl.texCoords.x/*u*/ = u_center - (u_wide * this->uv_scale_X);
		q->br.texCoords.x/*u*/ = u_center + (u_wide * this->uv_scale_X);
	}
	if (this->flags & PART_FLAG_V_SCALE){
		q->tl.texCoords.y/*v*/ = v_center - (v_height * this->uv_scale_Y);
		q->tr.texCoords.y/*v*/ = v_center - (v_height * this->uv_scale_Y);
		q->bl.texCoords.y/*v*/ = v_center + (v_height * this->uv_scale_Y);
		q->br.texCoords.y/*v*/ = v_center + (v_height * this->uv_scale_Y);
	}

	//UV反転
	if (this->flags & PART_FLAG_FLIP_H){	//左右反転を行う場合はlr入れ替え
		std::swap(q->tr.texCoords.x/*u*/, q->tl.texCoords.x/*u*/);
		std::swap(q->br.texCoords.x/*u*/, q->bl.texCoords.x/*u*/);
	}
	if (this->flags & PART_FLAG_FLIP_H){	//上下反転を行う場合はtb入れ替え
		std::swap(q->tr.texCoords.y/*v*/, q->br.texCoords.y/*v*/);
		std::swap(q->tl.texCoords.y/*v*/, q->bl.texCoords.y/*v*/);
	}
}


void State::vertexCompute(SSV3F_C4B_T2F_Quad* q, const SSRect& cellRect/*, const SSQuad3& vertexTransform*/) const
{
	//頂点を設定する
	float width_h = cellRect.width() / 2;
	float height_h = cellRect.height() / 2;
	float x1 = -width_h;
	float y1 = -height_h;
	float x2 = width_h;
	float y2 = height_h;

#ifdef UP_MINUS
	q->tl.vertices.x = x1;
	q->tl.vertices.y = y1;
	q->tr.vertices.x = x2;
	q->tr.vertices.y = y1;
	q->bl.vertices.x = x1;
	q->bl.vertices.y = y2;
	q->br.vertices.x = x2;
	q->br.vertices.y = y2;
#else
	q->tl.vertices.x = x1;
	q->tl.vertices.y = y2;
	q->tr.vertices.x = x2;
	q->tr.vertices.y = y2;
	q->bl.vertices.x = x1;
	q->bl.vertices.y = y1;
	q->br.vertices.x = x2;
	q->br.vertices.y = y1;
#endif

	//サイズ設定
	//頂点をサイズに合わせて変形させる
	if (this->flags & PART_FLAG_SIZE_X)
	{
		float w = 0;
		float center = 0;
		w = (q->tr.vertices.x - q->tl.vertices.x) / 2.0f;
		if (w!= 0.0f)
		{
			center = q->tl.vertices.x + w;
			float scale = (this->size_X / 2.0f) / w;

			q->bl.vertices.x = center - (w * scale);
			q->br.vertices.x = center + (w * scale);
			q->tl.vertices.x = center - (w * scale);
			q->tr.vertices.x = center + (w * scale);
		}
	}
	if (this->flags & PART_FLAG_SIZE_Y)
	{
		float h = 0;
		float center = 0;
		h = (q->bl.vertices.y - q->tl.vertices.y) / 2.0f;
		if (h != 0.0f)
		{
			center = q->tl.vertices.y + h;
			float scale = (this->size_Y / 2.0f) / h;

			q->bl.vertices.y = center - (h * scale);
			q->br.vertices.y = center - (h * scale);
			q->tl.vertices.y = center + (h * scale);
			q->tr.vertices.y = center + (h * scale);
		}
	}
}



} //namespace ss
