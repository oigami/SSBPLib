#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include "math/Vector2.h"
#include "player/Util.h"

namespace ss
{
typedef unsigned int u32;
typedef unsigned char u8;


///カラー値を定義するテンプレートクラスです。
template <typename T>
class SsTColor
{
public:
	T	r, g, b, a;

	SsTColor(): r(0), g(0), b(0), a(0) {}
	SsTColor(T ar, T ag, T ab, T aa): r(ar), g(ag), b(ab), a(aa) {}
	SsTColor(const SsTColor& s): r(s.r), g(s.g), b(s.b), a(s.a) {}

	void	fromARGB(u32 c);
	void	fromBGRA(u32 c);

	u32		toARGB() const;

	bool	operator ==(const SsTColor& rhs) const
	{
		return r == rhs.r
			&& g == rhs.g
			&& b == rhs.b
			&& a == rhs.a;
	}

private:
};


/// rgba 小数版
template<> inline SsTColor<float>::SsTColor(): r(0.5f), g(0.5f), b(0.5f), a(1.f) {}
template<> inline void SsTColor<float>::fromARGB(u32 c)
{
	a = (float)(c >> 24) / 255.f;
	r = (float)((c >> 16) & 0xff) / 255.f;
	g = (float)((c >> 8) & 0xff) / 255.f;
	b = (float)(c & 0xff) / 255.f;
}
template<> inline void SsTColor<float>::fromBGRA(u32 c)
{
	b = (float)(c >> 24) / 255.f;
	g = (float)((c >> 16) & 0xff) / 255.f;
	r = (float)((c >> 8) & 0xff) / 255.f;
	a = (float)(c & 0xff) / 255.f;
}
template<> inline u32 SsTColor<float>::toARGB() const
{
	u32 c = (u8)(a * 255) << 24 | (u8)(r * 255) << 16 | (u8)(g * 255) << 8 | (u8)(b * 255);
	return c;
}




template<> inline SsTColor<u32>::SsTColor(): r(255), g(255), b(255), a(255) {}
template<> inline void SsTColor<u32>::fromARGB(u32 c)
{
	a = (c >> 24);
	r = ((c >> 16) & 0xff);
	g = ((c >> 8) & 0xff);
	b = (c & 0xff);
}
template<> inline void SsTColor<u32>::fromBGRA(u32 c)
{
	b = (c >> 24) ;
	g = ((c >> 16) & 0xff) ;
	r = ((c >> 8) & 0xff) ;
	a = (c & 0xff) ;
}
template<> inline u32 SsTColor<u32>::toARGB() const
{
	u32 c = (u8)(a) << 24 | (u8)(r) << 16 | (u8)(g) << 8 | (u8)(b);
	return c;
}



template<> inline SsTColor<u8>::SsTColor(): r(255), g(255), b(255), a(255) {}
template<> inline void SsTColor<u8>::fromARGB(u32 c)
{
	a = (c >> 24);
	r = ((c >> 16) & 0xff);
	g = ((c >> 8) & 0xff);
	b = (c & 0xff);
}
template<> inline void SsTColor<u8>::fromBGRA(u32 c)
{
	b = (c >> 24) ;
	g = ((c >> 16) & 0xff) ;
	r = ((c >> 8) & 0xff) ;
	a = (c & 0xff) ;
}
template<> inline u32 SsTColor<u8>::toARGB() const
{
	u32 c = (u8)(a) << 24 | (u8)(r) << 16 | (u8)(g) << 8 | (u8)(b);
	return c;
}


///floatでのカラー値定義
typedef SsTColor<float> SsFColor;


typedef SsTColor<u8> SsU8Color;



//---------------------------------------------------------------

/// テクスチャラップモード
enum class SsTexWrapMode{
//	invalid = -1,	/// なし
	clamp,			/// クランプする
	repeat,			/// リピート
	mirror,			/// ミラー
//		num
};


/// テクスチャフィルターモード 画素補間方法
enum class SsTexFilterMode{
//	invalid = -1,
	nearlest,	///< ニアレストネイバー
	linear,		///< リニア、バイリニア
//	num
};



//エフェクト関連の定義
//エフェクトのノードタイプ
enum class SsEffectNodeType{
//	invalid=-1,
	root,
	emmiter,
	particle,
//	num
};



//エフェクト関連の定義
//エフェクトのブレンドタイプ
enum class SsRenderBlendType{
//	invalid=-1,
	Mix,
	Add,
//	num
};



} //namespace ss

