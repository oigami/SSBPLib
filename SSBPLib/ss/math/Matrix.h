#pragma once
#include <array>
#include "Vector3.h"

namespace ss{

/**
 * 行列計算
 *	vec * matrix --> vec の形。vecは横ベクトル
 */
class Matrix{
public:
	Matrix();

	std::array<float, 16> _m;
	
	
	//各種行列に整える
	Matrix& setupTranslation(float x, float y, float z=0);
	Matrix& setupRotationX(float radian);
	Matrix& setupRotationY(float radian);
	Matrix& setupRotationZ(float radian);
	Matrix& setupScale(float x, float y, float z=1);
	Matrix& setupIdentity();
	Matrix& setupSRzyxT(Vector3 scale, Vector3 rotationRadian, Vector3 translation);	//回転はZYX順でかけます
	
	//平行移動だけは簡単な演算で済むので別枠で定義しておく
	void setTranslation(float x, float y, float z=0);	//平行移動成分をxyzで置き換え
	void zeroTranslation();								//平行移動成分を0移動に置き換え
	void addTranslation(float x, float y, float z=0);	//平行移動させる
	Vector3 getTranslation() const;						//平行移動成分を取り出す

	//スケール値の抽出(SRT行列限定)
	Vector3 getScale() const;

	//回転の抽出(SRT行列限定)
	Vector3 getRotation() const;			//ラジアン返します
	Matrix getRotationMatrix() const;		//回転成分だけ行列として取り出す

	Matrix operator*(const Matrix& o) const;
	Matrix& operator*=(const Matrix& o);
	
};


Vector3 operator*(const Vector3& p, const Matrix& m);
Vector3& operator*=(Vector3& p, const Matrix& m);


} // namespace ss


