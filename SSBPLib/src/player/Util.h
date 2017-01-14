#pragma once

namespace ss{
/**
 * íËêî
 */

static const double SS_PI = 3.14159265358979323846;
static const double SS_2PI = SS_PI * 2;
static double SSRadToDeg(double rad){
	return rad * (180.0 / SS_PI);
}
static double SSDegToRad(double deg){
	return deg * (SS_PI / 180.0);
}


#define SS_SAFE_DELETE(p)           { delete (p); (p) = nullptr; }
#define SS_SAFE_DELETE_ARRAY(p)     { delete[] (p); (p) = nullptr; }


#ifdef _DEBUG
#define SSLOG(...)       do {} while (0)
#define SS_ASSERT(cond)    assert(cond)
#define SS_ASSERT2(cond, msg) SS_ASSERT(cond)
#define SSLOGERROR(format,...)  do {} while (0)
#else
#define SSLOG(...)       do {} while (0)
#define SS_ASSERT(cond)
#define SS_ASSERT2(cond, msg) ((void)(cond))
#define SSLOGERROR(format,...)  do {} while (0)
#endif


} //namespace ss
