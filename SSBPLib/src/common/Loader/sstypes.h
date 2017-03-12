#ifndef __SSTYPES__
#define __SSTYPES__

#include <stdlib.h>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include "math/Vector2.h"

namespace ss
{

//===============================================================
// Declare Type
//===============================================================


///２次元座標を表現するためのクラスです。
struct SsPoint2
{
#if 0
public:
	float	x;
	float	y;

public:
	SsPoint2( float _x , float _y )
	{
		x = _x ; y = _y;
	}
	SsPoint2() : x(0) , y(0){}

	static	float	distance_sq(const SsPoint2 & l, const SsPoint2 & r)
	{
		float x = l.x - r.x;
		float y = l.y - r.y;
		float sq = x * x + y * y;
		return sq;		
	}

	static	float	distance(const SsPoint2 & l, const SsPoint2 & r)
	{
		return  sqrt( distance_sq(l, r) );
	}

	SsPoint2 operator +( const SsPoint2& r) const
	{
		return SsPoint2(x + r.x, y + r.y);
	}

	SsPoint2 operator -( const SsPoint2& r) const
	{
		return SsPoint2(x - r.x, y - r.y);
	}

	SsPoint2 operator *( float r) const
	{
		return SsPoint2(x * r, y * r);
	}

	SsPoint2 operator /( float r) const
	{
		return SsPoint2(x / r, y / r);
	}

	float	length_sq() const
	{
		return (x * x) + (y * y);
	}

	float	length() const
	{
#ifdef _WIN32
		float r = length_sq();
		if ( r < 0.0001f && r > -0.0001f ) return 0;

		return (float)std::sqrtf( r );
#else
		return (float)std::sqrt( length_sq() );
#endif
	}

	static void	normalize(const SsPoint2& in, SsPoint2* out)
	{

		float len = in.length();
		float div = 0;

		if ( len == 0 )
		{
			div = 0;
		}else{
			div = (float)1 / in.length();
		}

		out->x = in.x * div;
		out->y = in.y * div;
	}

	void		normalize()
	{
		normalize(*this, this);
	}

	//----------------------------------------------------------------------------
	static	float	dot(const SsPoint2& l, const SsPoint2 r)
	{
		return (l.x * r.x) + (l.y * r.y);
	}
	static	float	cross(const SsPoint2& l, const SsPoint2& r)
	{
		return (l.x * r.y) - (l.y * r.x);
	}
#endif
	//----------------------------------------------------------------------------
	/**
		２つのベクトルが織りなす角度を求める
		入力は単位ベクトルでなくてはいけない。
	*/
	//----------------------------------------------------------------------------
	static	float 	get_angle_unit(Vector2 v0, Vector2 v1)
	{
		float ip = v0*v1;
		if (ip > 1.0f) ip = 1.0f;
		if (ip < -1.0f) ip = -1.0f;
		float f = acos(ip);
		return f;		
		
	}
	//----------------------------------------------------------------------------
	/**
		２つのベクトルが織りなす角度を求める
	*/
	//----------------------------------------------------------------------------
	static	float 	get_angle(Vector2 v0, Vector2 v1)
	{
		v0.normalize();
		v1.normalize();
		return get_angle_unit(v0, v1);
	}

	// v0 から v1 への左回りの角度を返す
	static	float 	get_angle_360_unit(Vector2 v0, Vector2 v1)
	{
		float ang = get_angle_unit(v0, v1);
		float c = v0.cross(v1);

		if (c < 0)
		{
			ang = (3.1415926535897932385f)*2.0f - ang;
		}
		return ang;
	}

	static	float 	get_angle_360(Vector2 v0, Vector2 v1)
	{
		v0.normalize();
		v1.normalize();
		return get_angle_360_unit(v0, v1);
	}

};

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


///unsigned intでのカラー値定義
typedef SsTColor<u32> SsColor;


typedef SsTColor<u8> SsU8Color;



//---------------------------------------------------------------

/// テクスチャラップモード
namespace SsTexWrapMode
{
	enum _enum
	{
		invalid = -1,	/// なし
		clamp,			/// クランプする
		repeat,			/// リピート
		mirror,			/// ミラー
		num
	};
};


/// テクスチャフィルターモード 画素補間方法
namespace SsTexFilterMode
{
	enum _enum
	{
		invalid = -1,
		nearlest,	///< ニアレストネイバー
		linear,		///< リニア、バイリニア
		num
	};
};



//エフェクト関連の定義
//エフェクトのノードタイプ
namespace SsEffectNodeType
{
	enum _enum{
		invalid=-1,
		root,
		emmiter,
		particle,
		num
	};
};



//エフェクト関連の定義
//エフェクトのブレンドタイプ
namespace SsRenderBlendType
{
	enum _enum{
		invalid=-1,
		Mix,
		Add,
		num
	};
};



};

#endif
