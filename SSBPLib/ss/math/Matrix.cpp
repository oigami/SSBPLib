#include "Matrix.h"
#include <math.h>
#include <algorithm>
#include <array>
#include "Vector3.h"
#include "player/Util.h"


namespace ss{

Matrix::Matrix(){
	setupIdentity();
}


/* 各種行列に整える ----------*/
Matrix& Matrix::setupTranslation(float x, float y, float z){
	_m[0] = 1.0f;	_m[1] = 0.0f;	_m[2] = 0.0f;	_m[3] = 0.0f;
	_m[4] = 0.0f;	_m[5] = 1.0f;	_m[6] = 0.0f;	_m[7] = 0.0f;
	_m[8] = 0.0f;	_m[9] = 0.0f;	_m[10] = 1.0f;	_m[11] = 0.0f;
	_m[12] = x;		_m[13] = y;		_m[14] = z;		_m[15] = 1.0f;
	return *this;
}

Matrix& Matrix::setupRotationX(float radian){
	float s = sin(radian);
	float c = cos(radian);
	
	_m[0] = 1.0f;	_m[1] = 0.0f;	_m[2] = 0.0f;	_m[3] = 0.0f;
	_m[4] = 0.0f;	_m[5] = c;		_m[6] = s;		_m[7] = 0.0f;
	_m[8] = 0.0f;	_m[9] = -s;		_m[10] = c;		_m[11] = 0.0f;
	_m[12] = 0.0f;	_m[13] = 0.0f;	_m[14] = 0.0f;	_m[15] = 1.0f;
	return *this;
}

Matrix& Matrix::setupRotationY(float radian){
	float s = sin(radian);
	float c = cos(radian);

	_m[0] = c;		_m[1] = 0.0f;	_m[2] = -s;		_m[3] = 0.0f;
	_m[4] = 0.0f;	_m[5] = 1.0f;	_m[6] = 0.0f;	_m[7] = 0.0f;
	_m[8] = s;		_m[9] = 0.0f;	_m[10] = c;		_m[11] = 0.0f;
	_m[12] = 0.0f;	_m[13] = 0.0f;	_m[14] = 0.0f;	_m[15] = 1.0f;
	return *this;
}

Matrix& Matrix::setupRotationZ(float radian){
	float s = sin(radian);
	float c = cos(radian);

	_m[0] = c;		_m[1] = s;		_m[2] = 0.0f;	_m[3] = 0.0f;
	_m[4] = -s;		_m[5] = c;		_m[6] = 0.0f;	_m[7] = 0.0f;
	_m[8] = 0.0f;	_m[9] = 0.0f;	_m[10] = 1.0f;	_m[11] = 0.0f;
	_m[12] = 0.0f;	_m[13] = 0.0f;	_m[14] = 0.0f;	_m[15] = 1.0f;
	return *this;
}

Matrix& Matrix::setupScale(float x, float y, float z){
	_m[0] = x;		_m[1] = 0.0f;	_m[2] = 0.0f;	_m[3] = 0.0f;
	_m[4] = 0.0f;	_m[5] = y;		_m[6] = 0.0f;	_m[7] = 0.0f;
	_m[8] = 0.0f;	_m[9] = 0.0f;	_m[10] = z;		_m[11] = 0.0f;
	_m[12] = 0.0f;	_m[13] = 0.0f;	_m[14] = 0.0f;	_m[15] = 1.0f;
	return *this;
}
	
Matrix& Matrix::setupIdentity(){
	_m[0] = 1.0f;	_m[1] = 0.0f;	_m[2] = 0.0f;	_m[3] = 0.0f;
	_m[4] = 0.0f;	_m[5] = 1.0f;	_m[6] = 0.0f;	_m[7] = 0.0f;
	_m[8] = 0.0f;	_m[9] = 0.0f;	_m[10] = 1.0f;	_m[11] = 0.0f;
	_m[12] = 0.0f;	_m[13] = 0.0f;	_m[14] = 0.0f;	_m[15] = 1.0f;
	return *this;
}
	
Matrix& Matrix::setupSRzyxT(Vector3 scale, Vector3 rotationRadian, Vector3 translation){
#if 0
	Matrix tmp;
	setupIdentity();
	*this *= tmp.setupScale(scale.x, scale.y, scale.z);
	*this *= tmp.setupRotationZ(rotationRadian.z);
	*this *= tmp.setupRotationY(rotationRadian.y);
	*this *= tmp.setupRotationX(rotationRadian.x);
	*this *= tmp.setupTranslation(translation.x, translation.y, translation.z);
#endif
	float sin_x = sin(rotationRadian.x);
	float cos_x = cos(rotationRadian.x);
	float sin_y = sin(rotationRadian.y);
	float cos_y = cos(rotationRadian.y);
	float sin_z = sin(rotationRadian.z);
	float cos_z = cos(rotationRadian.z);

	_m[0] = scale.x * (cos_z * cos_y);
	_m[1] = scale.x * (sin_z * cos_x  +  cos_z * sin_y * sin_x);
	_m[2] = scale.x * (sin_z * sin_x  -  cos_z * sin_y * cos_x);
	_m[3] = 0;

	_m[4] = scale.y * (-sin_z * cos_y);
	_m[5] = scale.y * (cos_z * cos_x  -  sin_z * sin_y * sin_x);
	_m[6] = scale.y * (cos_z * sin_x  +  sin_z * sin_y * cos_x);
	_m[7] = 0;

	_m[8] = scale.z * sin_y;
	_m[9] = scale.z * (-cos_y * sin_x);
	_m[10] = scale.z * (cos_y * cos_x);
	_m[11] = 0;

	_m[12] = translation.x;
	_m[13] = translation.y;
	_m[14] = translation.z;
	_m[15] = 1.0f;

	return *this;
}



/* 平行移動だけは簡単な演算で済むので別枠で定義しておく ----------*/
void Matrix::setTranslation(float x, float y, float z){			//平行移動成分をxyzで置き換え
	_m[12] = x;		_m[13] = y;		_m[14] = z;		//_m[15] = 1.0f;
}

void Matrix::zeroTranslation(){									//平行移動成分を0移動に置き換え
	setTranslation(0,0,0);
}

void Matrix::addTranslation(float x, float y, float z){			//平行移動させる
	//memo:作業内容はthis * setuptransration(x,y,z) なのだが、計算するとここの成分しか変わらない
	_m[12] += x;	_m[13] += y;	_m[14] += z;	//_m[15] = 1.0f;
}

Vector3 Matrix::getTranslation() const{							//平行移動成分を取り出す
	return Vector3(_m[12], _m[13], _m[14]);
}

/* スケール ------------------------------------------------------*/
Vector3 Matrix::getScale() const
{
	return Vector3(
		sqrt( _m[0]*_m[0] + _m[1]*_m[1] + _m[2]*_m[2] ),
		sqrt( _m[4]*_m[4] + _m[5]*_m[5] + _m[6]*_m[6] ),
		sqrt( _m[8]*_m[8] + _m[9]*_m[9] + _m[10]*_m[10] )
	);
}

/* 回転 ----------------------------------------------------------*/
Vector3 Matrix::getRotation() const
{
	static const float THRESHOLD = 0.001;
	//ZYXの順の回転行列のをオイラー角に戻す
	
	//回転行列だけにして考える
	Matrix rotMat = getRotationMatrix();
	float sin_y = rotMat._m[8];
	
	float x, y, z;
	if(abs(sin_y - 1) < THRESHOLD){			//sin_y == 1
		x = 0;
		y = SS_PI / 2;
		z = atan2(rotMat._m[1], rotMat._m[5]);
	}
	else if(abs(sin_y + 1) < THRESHOLD){	//sin_y == -1
		x = 0;
		y = -SS_PI / 2;
		z = atan2(rotMat._m[1], rotMat._m[5]);
	}
	else{
		x = atan2(-rotMat._m[9], rotMat._m[10]);
		y = asin(sin_y);
		z = atan2(-rotMat._m[4], rotMat._m[0]);
	}
	return Vector3(x, y, z);
}

Matrix Matrix::getRotationMatrix() const
{
	/*
	  スケールS, 回転R, 平行移動T の行列はこうなってる
	  
	        SxR00, SxR01, SxR02, 0
	  mat = SyR10, SyR11, SyR12, 0
	        SzR20, SzR21, SzR22, 0
			Tx,    Ty,    Tz,    1
	  
	  要素を打ち消してR成分だけにする
	*/
	Matrix rotMat = *this;
	rotMat.setTranslation(0, 0, 0);
	
	Vector3 s = rotMat.getScale();
	s.x = (s.x==0)? s.x : 1.0f/s.x;		//0除算回避。スケール0なら行が0,0,0になってるはず
	s.y = (s.y==0)? s.y : 1.0f/s.y;
	s.z = (s.z==0)? s.z : 1.0f/s.z;
	rotMat._m[0] *= s.x;	rotMat._m[1] *= s.x; 	rotMat._m[2] *= s.x;
	rotMat._m[4] *= s.y;	rotMat._m[5] *= s.y; 	rotMat._m[6] *= s.y;
	rotMat._m[8] *= s.z;	rotMat._m[9] *= s.z; 	rotMat._m[10] *= s.z;

	return rotMat;
}


/* 演算子の定義 ----------*/
Matrix Matrix::operator*(const Matrix& o) const{
	Matrix tmp;

	tmp._m[0]  = _m[0]*o._m[0] + _m[1]*o._m[4] + _m[2]*o._m[8] + _m[3]*o._m[12];
	tmp._m[1]  = _m[0]*o._m[1] + _m[1]*o._m[5] + _m[2]*o._m[9] + _m[3]*o._m[13];
	tmp._m[2]  = _m[0]*o._m[2] + _m[1]*o._m[6] + _m[2]*o._m[10] + _m[3]*o._m[14];
	tmp._m[3]  = _m[0]*o._m[3] + _m[1]*o._m[7] + _m[2]*o._m[11] + _m[3]*o._m[15];

	tmp._m[4]  = _m[4]*o._m[0] + _m[5]*o._m[4] + _m[6]*o._m[8] + _m[7]*o._m[12];
	tmp._m[5]  = _m[4]*o._m[1] + _m[5]*o._m[5] + _m[6]*o._m[9] + _m[7]*o._m[13];
	tmp._m[6]  = _m[4]*o._m[2] + _m[5]*o._m[6] + _m[6]*o._m[10] + _m[7]*o._m[14];
	tmp._m[7]  = _m[4]*o._m[3] + _m[5]*o._m[7] + _m[6]*o._m[11] + _m[7]*o._m[15];
	
	tmp._m[8]  = _m[8]*o._m[0] + _m[9]*o._m[4] + _m[10]*o._m[8] + _m[11]*o._m[12];
	tmp._m[9]  = _m[8]*o._m[1] + _m[9]*o._m[5] + _m[10]*o._m[9] + _m[11]*o._m[13];
	tmp._m[10] = _m[8]*o._m[2] + _m[9]*o._m[6] + _m[10]*o._m[10] + _m[11]*o._m[14];
	tmp._m[11] = _m[8]*o._m[3] + _m[9]*o._m[7] + _m[10]*o._m[11] + _m[11]*o._m[15];
	//平行移動成分
	tmp._m[12] = _m[12]*o._m[0] + _m[13]*o._m[4] + _m[14]*o._m[8] + _m[15]*o._m[12];
	tmp._m[13] = _m[12]*o._m[1] + _m[13]*o._m[5] + _m[14]*o._m[9] + _m[15]*o._m[13];
	tmp._m[14] = _m[12]*o._m[2] + _m[13]*o._m[6] + _m[14]*o._m[10] + _m[15]*o._m[14];
	tmp._m[15] = _m[12]*o._m[3] + _m[13]*o._m[7] + _m[14]*o._m[11] + _m[15]*o._m[15];

	return tmp;
}

Matrix& Matrix::operator*=(const Matrix& o){
	*this = *this * o;
	return *this;
}



//頂点変換
Vector3 operator*(const Vector3& p, const Matrix& m){

	//(x,y,z,w=1)*matrix の計算。実際にはwはないのでごまかしつつ計算
	return Vector3(
		p.x*m._m[0] + p.y*m._m[4] + p.z*m._m[8] + m._m[12],
		p.x*m._m[1] + p.y*m._m[5] + p.z*m._m[9] + m._m[13],
		p.x*m._m[2] + p.y*m._m[6] + p.z*m._m[10] + m._m[14]
	);
}

Vector3& operator*=(Vector3& p, const Matrix& m){
	p = p * m;
	return p;
}



} //namespace ss
