#pragma once
#include <algorithm>
#include <assert.h>

namespace ss{
/**
 * 定数
 */

static const double SS_PI = 3.14159265358979323846;
static const double SS_2PI = SS_PI * 2;
double SSRadToDeg(double rad);
double SSDegToRad(double deg);

void DebugPrintToConsole(const char *filename, int line, const char *format, ...);


/** 配列のサイズを返す */
template<class TYPE, size_t N>
size_t lengthof(const TYPE(&ar)[N]){
	return N;
}

/** valを[min:max]の範囲にしたものを返す */
template<class T>
T clamp(T val, T minVal, T maxVal){
	assert(minVal <= maxVal);
	return std::min(std::max(val, minVal), maxVal);
}

/** [minVal:maxVal)の範囲でループさせる。整数値向け */
template<class T>
T wrap(T val, T minVal, T maxVal){
	assert(minVal < maxVal);
	int n = (val - minVal) % (maxVal - minVal);
	return (n >= 0) ? (n + minVal) : (n + maxVal);
}

/** [minVal:maxVal)の範囲でループさせる。浮動小数点数向け */
template<class T>
T fwrap(T val, T minVal, T maxVal){
	assert(minVal < maxVal);
	double n = fmod(val - minVal, maxVal - minVal);
	return (n >= 0) ? (n + minVal) : (n + maxVal);
}


/** 線形補間 t[0:1] */
template <typename T>
T lerp(const T &from, const T &to, double t){
//	T diff = to - from;
//	return from + diff*t;
	return from * (1.0 - t) + to * t;
}

/** 角度を[0:360)の範囲で補完する t[0:1] */
float lerpAngle(float from, float to, double t);


} //namespace ss



#define SS_SAFE_DELETE(p)           do{ delete (p); (p) = nullptr; }while(0)
#define SS_SAFE_DELETE_ARRAY(p)     do{ delete[] (p); (p) = nullptr; }while(0)


#ifdef _DEBUG
#define SS_LOG(...)					ss::DebugPrintToConsole(__FILE__, __LINE__, __VA_ARGS__)
#define SS_ASSERT(cond)				do{ if(!(cond)){ SS_LOG(#cond); abort(); } }while(0)
#define SS_ASSERT_LOG(cond, ...)	do{ if(!(cond)){ SS_LOG(__VA_ARGS__); abort(); } }while(0)
#else
#define SS_LOG(...)
#define SS_ASSERT(cond)				
#define SS_ASSERT_LOG(cond, ...)	
#endif
