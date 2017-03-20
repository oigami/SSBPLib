#ifndef __SSEFFECTBEHAVIOR__
#define __SSEFFECTBEHAVIOR__

#include "SsEffectElement.h"
#include "sstypes.h"
#include "player/CellCache.h"

namespace ss
{

class SsEffectElementBase;

class SsEffectBehavior{
public:
	std::vector<const SsEffectElementBase*> plist;

private:
	int					m_cellIndex;	//プレイヤー専用に追加
	const CellRef*		m_cellRef;
	SsRenderBlendType	m_blendType;

public:
	SsEffectBehavior(int cellIndex, const CellRef* cellRef, SsRenderBlendType blendType)
		: m_cellIndex(cellIndex)
		, m_cellRef(cellRef)
		, m_blendType(blendType)
	{}
	
	~SsEffectBehavior(){}

	int getCellIndex() const{ return m_cellIndex; }
	const CellRef* getCellRef() const{ return m_cellRef; }
	SsRenderBlendType getBlendType() const{ return m_blendType; }
};


};



#endif
