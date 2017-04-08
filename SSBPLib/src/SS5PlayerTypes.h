#pragma once

#include <float.h>
#include <math.h>
#include <assert.h>
#include "player/Util.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

namespace ss{
class DataArrayReader;
struct SSColorF;

using TextureID = long;	//テクスチャの識別子
static const TextureID TEXTURE_ID_INVALID = -1;	//無効なテクスチャID


/** カラー構造体 */
struct SSColor4B {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	using uchar = unsigned char;
	SSColor4B(uchar r_, uchar g_, uchar b_, uchar a_) :r(r_), g(g_), b(b_), a(a_){}
	SSColor4B() : SSColor4B(255, 255, 255, 255){}
	
	//カラーの読み取り
	void readColor(DataArrayReader& reader);
	//カラーの読み取り(rateを考慮して読む)
	void readColorWithRate(DataArrayReader& reader);

	//unsigned longにパラメータを固める
	unsigned long packARGB() const;
	unsigned long packRGBA() const;
	unsigned long packABGR() const;
	unsigned long packBGRA() const;

	//float版への変換
	SSColorF toSSColorF() const;
};

/** [0:1]範囲でのカラー表現 */
struct SSColorF {
	float r;
	float g;
	float b;
	float a;
	SSColorF(float r_, float g_, float b_, float a_) :r(r_), g(g_), b(b_), a(a_){}
	SSColorF() : SSColorF(1.0f, 1.0f, 1.0f, 1.0f){}
	
	//カラーの読み取り
	void readColor(DataArrayReader& reader);
	//カラーの読み取り(rateを考慮して読む)
	void readColorWithRate(DataArrayReader& reader);

	//unsigned longにパラメータを固める
	unsigned long packARGB() const;
	unsigned long packRGBA() const;
	unsigned long packABGR() const;
	unsigned long packBGRA() const;

	//uchar版への変換
	SSColor4B toSSColor4B() const;
};


/**
 * ４頂点座標構造体
 */
struct SSQuad3 {
	Vector3 bl;
	Vector3 br;
	Vector3 tl;
	Vector3 tr;

	//頂点オフセットの読み取り
	void readVertexTransform(DataArrayReader& reader);
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

	//4頂点座標の加算
	void add(const SSQuad3& positions){
		tl.vertices += positions.tl;
		bl.vertices += positions.bl;
		tr.vertices += positions.tr;
		br.vertices += positions.br;
	}

	//vertex各要素に演算を適用させる
	template<class F>
	void vertexForeach(F func){	//[](Vector3& v){}
		func(tl.vertices);
		func(tr.vertices);
		func(bl.vertices);
		func(br.vertices);
	}

	//uv各要素に演算を適用させる
	template<class F>
	void uvForeach(F func){		//[](SSTex2F& uv){}
		func(tl.texCoords);
		func(tr.texCoords);
		func(bl.texCoords);
		func(br.texCoords);
	}

	//color各要素に演算を適用させる
	template<class F>
	void colorsForeach(F func){	//[](SSColor4B& color){}
		func(tl.colors);
		func(tr.colors);
		func(bl.colors);
		func(br.colors);
	}
};


};	// namespace ss

