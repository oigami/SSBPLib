#include "EffectPartStatus.h"
#include "DataArrayReader.h"
#include "PlayerDef.h"
#include "SS5PlayerData.h"

namespace ss{

EffectPartStatus::EffectPartStatus()
	: m_curKeyframe(0)
	, m_startTime(0)
	, m_speed(0)
	, m_independent(false)
{
	/**/
}

void EffectPartStatus::readData(int readFlags, DataArrayReader &reader, const AnimationInitialData* init)
{
	m_curKeyframe	= readFlags & PART_FLAG_EFFECT_KEYFRAME ? reader.readS32() : init->effectValue_curKeyframe;
	m_startTime		= readFlags & PART_FLAG_EFFECT_KEYFRAME ? reader.readS32() : init->effectValue_startTime;
	m_speed			= readFlags & PART_FLAG_EFFECT_KEYFRAME ? reader.readFloat() : init->effectValue_speed;
	
	int loopflag	= readFlags & PART_FLAG_EFFECT_KEYFRAME ? reader.readS32() : init->effectValue_loopflag;
	m_independent = (loopflag & EFFECT_LOOP_FLAG_INDEPENDENT);
}


} //namespace ss
