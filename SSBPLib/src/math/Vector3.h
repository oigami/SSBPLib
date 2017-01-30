#pragma once
#include <math.h>
#include <algorithm>
#include <assert.h>
#include "player/Util.h"
#include "Vector2.h"

namespace ss{

/** 3次元ベクトル */
class Vector3{
public:
	float x;
	float y;
	float z;

	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 right;
	static const Vector3 left;
	static const Vector3 forward;
	static const Vector3 back;
	static const Vector3 one;
	static const Vector3 zero;

	Vector3() :x(0.0f), y(0.0f), z(0.0f){}
	Vector3(float fx, float fy, float fz) : x(fx), y(fy), z(fz){}
	Vector3(const Vector3& o) : x(o.x), y(o.y), z(o.z){}

	Vector3& operator =(const Vector3& o){
		x = o.x;	y = o.y;	z = o.z;
		return *this;
	}
	bool operator ==(const Vector3& o) const{
		return (x == o.x && y == o.y && z == o.z);
	}
	bool operator !=(const Vector3& o) const{
		return !(*this == o);
	}

	Vector3 operator -() const{
		return Vector3(-x, -y, -z);
	}
	Vector3 operator +(const Vector3& o) const{
		return Vector3(x + o.x, y + o.y, z + o.z);
	}
	Vector3 operator -(const Vector3& o) const{
		return Vector3(x - o.x, y - o.y, z - o.z);
	}
	float operator *(const Vector3& o) const{	//内積
		return x*o.x + y*o.y + z*o.z;
	}
	Vector3 operator *(float s) const{
		return Vector3(x*s, y*s, z*s);
	}
	Vector3 operator /(float s) const{
		SS_ASSERT_LOG(s, "Vector3 division by 0.");	//ゼロ除算チェック
		float oneOverS = 1.0f / s;
		return Vector3(x*oneOverS, y*oneOverS, z*oneOverS);
	}

	Vector3& operator +=(const Vector3& o){
		x += o.x;	y += o.y;	z += o.z;
		return *this;
	}
	Vector3& operator -=(const Vector3& o){
		x -= o.x;	y -= o.y;	z -= o.z;
		return *this;
	}
	Vector3& operator *=(float s){
		x *= s;	y *= s;	z *= s;
		return *this;
	}
	Vector3& operator /=(float s){
		SS_ASSERT_LOG(s, "Vector3 division by 0.");	//ゼロ除算チェック
		float oneOverS = 1.0f / s;
		x *= oneOverS;	y *= oneOverS;	z *= oneOverS;
		return *this;
	}


	//! ベクトルの長さ
	float length() const {
		return sqrt(x*x + y*y + z*z);
	}
	//! ベクトルの長さの2乗(lengthでsqrt()をとらない)
	float lengthSquare() const {
		return x*x + y*y + z*z;
	}
	//! 距離
	float distance(const Vector3& o) const{
		return (*this - o).length();
	}
	//! 距離の2乗
	float distanceSquare(const Vector3& o) const{
		return (*this - o).lengthSquare();
	}

	//! 正規化する
	void normalize() {
		float mag = x*x + y*y + z*z;
		if(mag > 0.0f) {
			float oneOverMag = 1.0f / sqrt(mag);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}

	//! 外積
	Vector3 cross(const Vector3& o) const {
		return Vector3(
			y*o.z - z*o.y,
			z*o.x - x*o.z,
			x*o.y - y*o.x
		);
	}


	//! Vector2への変換
	Vector2 xy() const{
		return Vector2(x, y);
	}
};

//! スカラー乗算の対称性のために定義しておく
inline const Vector3 operator *(float s, const Vector3& o){
	return Vector3(s*o.x, s*o.y, s*o.z);
}



} //namespace ss
