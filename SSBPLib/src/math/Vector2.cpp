#include "Vector2.h"

namespace ss {

const Vector2 Vector2::up(0.0f, 1.0f);
const Vector2 Vector2::down(0.0f, -1.0f);
const Vector2 Vector2::right(1.0f, 0.0f);
const Vector2 Vector2::left(-1.0f, 0.0f);
const Vector2 Vector2::one(1.0f, 1.0f);
const Vector2 Vector2::zero(0.0f, 0.0f);



namespace Vec2Util{

float getAngle360(Vector2 v0, Vector2 v1)
{
	v0.normalize();
	v1.normalize();

	//角度を求める
	float cos_val = clamp(v0 * v1, -1.0f, 1.0f);
	float angle = acos(cos_val);

	float c = v0.cross(v1);
	if(c < 0){
		return  SS_2PI - angle;
	}
	return angle;
}


} //namespace Vec2Util

} //namespace ss
