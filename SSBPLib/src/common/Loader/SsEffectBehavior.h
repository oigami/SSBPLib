﻿#ifndef __SSEFFECTBEHAVIOR__
#define __SSEFFECTBEHAVIOR__

#include "SsEffectElement.h"
#include "../Animator/ssplayer_cellmap.h"
#include "sstypes.h"
#include "player/CellCache.h"

namespace ss
{

class SsEffectElementBase;

class SsEffectBehavior{
public:
	std::vector<SsEffectElementBase*> plist;


	int								CellIndex;	//プレイヤー専用に追加
	const CellRef*					refCell;
	SsRenderBlendType::_enum		blendType;

public:
	SsEffectBehavior() : blendType(SsRenderBlendType::invalid) {}
	virtual ~SsEffectBehavior(){}

};


};



#endif
