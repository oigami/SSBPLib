#pragma once
#include <math.h>
#include <algorithm>
#include <assert.h>
#include "player/Util.h"

namespace ss{

/** 2次元ベクトル */
class Vector2 {
public:
	float x, y;
	float u() const { return x; }	//uv扱えるようにしておく
	float v() const { return y; }	//uv扱えるようにしておく
	float width() const { return x; }	//旧SSSize用
	float height() const { return y; }	//旧SSSize用

	static const Vector2 up;
	static const Vector2 down;
	static const Vector2 right;
	static const Vector2 left;
	static const Vector2 one;
	static const Vector2 zero;
	
	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(float fx, float fy) : x(fx), y(fy) {}
	Vector2(const Vector2& o) : x(o.x), y(o.y) {}
	
	Vector2& operator =(const Vector2& o) {
		x = o.x;
		y = o.y;
		return *this;
	}
	bool operator ==(const Vector2& o) const {
		return (x == o.x && y == o.y);
	}
	bool operator !=(const Vector2& o) const {
		return (x != o.x || y != o.y);
	}
	
	Vector2 operator -() const {
		return Vector2(-x, -y);
	}
	Vector2 operator +(const Vector2& o) const {
		return Vector2(x + o.x, y + o.y);
	}
	Vector2 operator -(const Vector2& o) const {
		return Vector2(x - o.x, y - o.y);
	}
	float   operator *(const Vector2& o) const {	//内積
		return x*o.x + y*o.y;
	}
	Vector2 operator *(float s) const {
		return Vector2(x*s, y*s);
	}
	Vector2 operator /(float s) const {
		SS_ASSERT_LOG(s, "Vector2 division by 0.");	//ゼロ除算チェック
		float oneOverS = 1.0f / s;
		return Vector2(x*oneOverS, y*oneOverS);
	}
	
	Vector2& operator +=(const Vector2& o) {
		x += o.x;
		y += o.y;
		return *this;
	}
	Vector2& operator -=(const Vector2& o) {
		x -= o.x;
		y -= o.y;
		return *this;
	}
	Vector2& operator *=(float s) {
		x *= s;
		y *= s;
		return *this;
	}
	Vector2& operator /=(float s) {
		SS_ASSERT_LOG(s, "Vector2 division by 0.");	//ゼロ除算チェック
		float oneOverS = 1.0f / s;
		x *= oneOverS;
		y *= oneOverS;
		return *this;
	}
	
	
	//! ベクトルの長さ
	float length() const {
		return sqrt(x*x + y*y);
	}
	//! ベクトルの長さの2乗(lengthでsqrt()をとらない)
	float lengthSquare() const {
		return x*x + y*y;
	}
	//! 距離
	float distance(const Vector2& o) const{
		return (*this - o).length();
	}
	//! 距離の2乗
	float distanceSquare(const Vector2& o) const{
		return (*this - o).lengthSquare();
	}

	//! 正規化する
	void normalize() {
		float mag = x*x + y*y;
		if (mag > 0.0f) {
			float oneOverMag = 1.0f / sqrt(mag);
			x *= oneOverMag;
			y *= oneOverMag;
		}
	}

	//! 2Dでの外積はスカラー値として定義
	float cross(const Vector2& o) const {
		return x*o.y - y*o.x;
	}

	//! 回転させる
	void rotate(float angle) {
		float tx = x;
		x = tx * cos(angle) - y * sin(angle);
		y = tx * sin(angle) + y * cos(angle);
	}
	//! 基準点を中心として回転させる
	void rotate(float angle, const Vector2& offset) {
		*this -= offset;
		rotate(angle);
		*this += offset;
	}
};

//! スカラー乗算の対称性のために定義しておく
inline const Vector2 operator *(float s, const Vector2& o){
	return Vector2(s*o.x, s*o.y);
}


using SSPoint = Vector2;
using SSSize = Vector2;



} //namespace ss

