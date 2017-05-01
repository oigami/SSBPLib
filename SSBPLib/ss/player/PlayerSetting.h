#pragma once
#include "math/Matrix.h"
#include "math/Vector3.h"
#include "SS5PlayerTypes.h"

namespace ss{

/** Playerが再生するパーツ全体に影響を与える設定を抱えておく */
class PlayerSetting{
public:
	PlayerSetting() :
		m_position(Vector3::zero),
		m_rotation(Vector3::zero),
		m_scale(Vector3::one),
		m_color(1.0f, 1.0f, 1.0f, 1.0f){}

	Matrix  m_parentMatrix;	//最初にかかる行列(普段は単位行列だが、親子付けするときなどは親の行列を設定する)
	Vector3 m_position;		//位置
	Vector3 m_rotation;		//回転(deg)
	Vector3 m_scale;		//スケール
	SSColorF m_color;		//rgbのついでにalphaも保持します


	Matrix getWorldMatrix() const{
		return m_parentMatrix * getLocalMatrix();
	}

private:
	Matrix getLocalMatrix() const{
		Matrix mat;
		Vector3 rotRadian = SSDegToRad(m_rotation);
		mat.setupSRzyxT(m_scale, rotRadian, m_position);
		return mat;
	}
};

} //namespace ss
