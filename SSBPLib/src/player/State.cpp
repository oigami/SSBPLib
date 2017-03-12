#include "State.h"
#include "DataArrayReader.h"
#include "PlayerDef.h"
#include "math/Matrix.h"
#include "InstancePartStatus.h"

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

	colorBlendFunc = BLEND_MIX;
	colorBlendType = 0;
	memset(&quad, 0, sizeof(quad));
	texture.handle = 0;
	texture.size_w = 0;
	texture.size_h = 0;
	rect = SSRect(0, 0, 0, 0);
	blendfunc = BLEND_MIX;
	mat.setupIdentity();

	instanceValue = InstancePartStatus();
	effectValue_curKeyframe = 0;
	effectValue_startTime = 0;
	effectValue_speed = 0;
	effectValue_loopflag = 0;

	Calc_opacity = 255;

}


void State::readData(DataArrayReader& reader, const AnimationInitialData* init)
{
	// optional parameters
	flags      = reader.readU32();
	cellIndex  = flags & PART_FLAG_CELL_INDEX ? reader.readS16() : init->cellIndex;
	x        = flags & PART_FLAG_POSITION_X ? reader.readFloat() : init->positionX;
	y        = flags & PART_FLAG_POSITION_Y ? reader.readFloat() : init->positionY;
	z        = flags & PART_FLAG_POSITION_Z ? reader.readFloat() : init->positionZ;
	pivotX   = flags & PART_FLAG_PIVOT_X ? reader.readFloat() : init->pivotX;
	pivotY   = flags & PART_FLAG_PIVOT_Y ? reader.readFloat() : init->pivotY;
	rotationX = flags & PART_FLAG_ROTATIONX ? reader.readFloat() : init->rotationX;
	rotationY = flags & PART_FLAG_ROTATIONY ? reader.readFloat() : init->rotationY;
	rotationZ = flags & PART_FLAG_ROTATIONZ ? reader.readFloat() : init->rotationZ;
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
	instanceValue.readData(flags, reader, init);

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
	

	//計算用にuv中心を求めておく
	//memo:回転・スケールで中央への戻し処理が多発するのでボトルネックになるようなら最初から中心合わせで計算して最後にオフセットを足すようにするのがいいかもしれない
	SSTex2F uvCenter = (q->br.texCoords + q->tl.texCoords) / 2.0f;
	
	//UV回転
	if (this->flags & PART_FLAG_UV_ROTATION){
		//頂点位置を回転させる
		float rotateRadian = SSDegToRad(this->uv_rotation);
		q->tl.texCoords.rotate(rotateRadian, uvCenter);
		q->tr.texCoords.rotate(rotateRadian, uvCenter);
		q->bl.texCoords.rotate(rotateRadian, uvCenter);
		q->br.texCoords.rotate(rotateRadian, uvCenter);
	}

	//UVスケール
	if(this->flags & (PART_FLAG_U_SCALE | PART_FLAG_V_SCALE)){
		q->uvForeach([&](SSTex2F &uv){
			//中心を基準として拡大縮小させる
			uv -= uvCenter;
			uv.x/*u*/ *= this->uv_scale_X;
			uv.y/*v*/ *= this->uv_scale_Y;
			uv += uvCenter;
		});
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
	if(this->flags & PART_FLAG_SIZE_X){
		q->br.vertices.x = this->size_X;
		q->tr.vertices.x = this->size_X;
	}
	if(this->flags & PART_FLAG_SIZE_Y){
		q->tl.vertices.y = this->size_Y;
		q->tr.vertices.y = this->size_Y;
	}

	//中心が(0,0)になるようにオフセットを追加
	Vector3 center = (q->bl.vertices + q->tr.vertices) / 2;
	q->bl.vertices -= center;
	q->br.vertices -= center;
	q->tl.vertices -= center;
	q->tr.vertices -= center;
}



} //namespace ss
