#include "Vector3.h"
#include "player/Util.h"

namespace ss{

const Vector3 Vector3::up(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::down(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::right(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::left(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::forward(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::back(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::one(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::zero(0.0f, 0.0f, 0.0f);


Vector3 SSRadToDeg(Vector3 rad){
	return Vector3(
		SSRadToDeg(rad.x),
		SSRadToDeg(rad.x),
		SSRadToDeg(rad.z)
	);
}
Vector3 SSDegToRad(Vector3 deg){
	return Vector3(
		SSDegToRad(deg.x),
		SSDegToRad(deg.x),
		SSDegToRad(deg.z)
	);
}


} //namespace ss
