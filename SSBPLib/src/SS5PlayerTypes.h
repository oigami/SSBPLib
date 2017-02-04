#pragma once

#include <float.h>
#include <math.h>
#include <assert.h>
#include "player/Util.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

namespace ss{
class DataArrayReader;


/**
 * カラー構造体
 */
struct SSColor4B {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	using uchar = unsigned char;
	SSColor4B(uchar r_, uchar g_, uchar b_, uchar a_) :r(r_), g(g_), b(b_), a(a_){}
	SSColor4B() : SSColor4B(0, 0, 0, 0){}
	
	//カラーの読み取り
	void readColor(DataArrayReader& reader);
	//カラーの読み取り(rateを考慮して読む)
	void readColorWithRate(DataArrayReader& reader);

	//unsigned longにパラメータを固める
	unsigned long packARGB() const;
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

