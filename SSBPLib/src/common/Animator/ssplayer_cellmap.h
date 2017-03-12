#ifndef __SSPLAYER_CELLMAP__
#define __SSPLAYER_CELLMAP__

#include "SS5PlayerTypes.h"
#include "math/SSRect.h"
#include "common/Loader/sstypes.h"
#include "player/CellCache.h"

namespace ss
{

///パーツが使用するセルの情報
struct SsCellValue
{
	SsRenderBlendType::_enum    blendType;
	const CellRef*				refCell;
	int cellIndex;


	SsCellValue() :  
		blendType(SsRenderBlendType::_enum::Add),
		cellIndex(-1)
		{}
};

};
#endif
