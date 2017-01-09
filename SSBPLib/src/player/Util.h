#pragma once

/**
 * ’è”
 */

#define __SSPI__	(3.14159265358979323846f)
#define __SS2PI__	(__SSPI__ * 2)
#define SSRadianToDegree(Radian) ((float)( Radian * __SS2PI__ )/ 360.0f )
#define SSDegreeToRadian(Degree) ((float)( Degree * 360.0f) / __SS2PI__)


#define SS_SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define SS_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define SS_SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)
#define SS_SAFE_RELEASE(p)            do { if(p) { (p)->release(); } } while(0)
#define SS_SAFE_RELEASE_NULL(p)        do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define SS_SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)
#define SS_BREAK_IF(cond)            if(cond) break

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

