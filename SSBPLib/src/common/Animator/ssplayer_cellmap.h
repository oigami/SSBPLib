#ifndef __SSPLAYER_CELLMAP__
#define __SSPLAYER_CELLMAP__

#include "SS5PlayerTypes.h"
#include "math/SSRect.h"
#include "common/Loader/sstypes.h"
#include "player/CellCache.h"

namespace ss
{

#if 0
/**
* SsCell
*/

struct SsCell
{
	float		pivot_X;		//原点補正
	float		pivot_Y;		//原点補正
	//TextuerData texture;
	int			cellMapIndex;	//セルマップ番号
	ss::SSRect	rect;
	//std::string texname;
	int			cellIndex;
//	std::string cellName;

	SsCell():
		  pivot_X(0)
		, pivot_Y(0)
		, cellIndex(-1)
	{}
};
#endif

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
