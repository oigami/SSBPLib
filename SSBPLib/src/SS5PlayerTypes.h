#pragma once

#include <float.h>
#include <math.h>
#include <assert.h>
#include "player/Util.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

namespace ss{
	
/**
 * カラー構造体
 */
struct SSColor4B {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};


/**
 * ４頂点座標構造体
 */
struct SSQuad3 {
	Vector3 bl;
	Vector3 br;
	Vector3 tl;
	Vector3 tr;
};


/**
 * 頂点構造体
 */
struct SSV3F_C4B_T2F {
	Vector3		vertices;	/// 座標
	SSColor4B	colors;		/// カラー
	SSTex2F		texCoords;	/// UV
};

/**
 * ４頂点構造体
 */
struct SSV3F_C4B_T2F_Quad {
	SSV3F_C4B_T2F tl;
	SSV3F_C4B_T2F bl;
	SSV3F_C4B_T2F tr;
	SSV3F_C4B_T2F br;
};

//テクスチャデータ
struct TextuerData
{
	long handle;
	int size_w;
	int size_h;
};


};	// namespace ss

