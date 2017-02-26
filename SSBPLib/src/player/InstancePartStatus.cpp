#include "InstancePartStatus.h"
#include "DataArrayReader.h"
#include "PlayerDef.h"
#include "SS5PlayerData.h"

namespace ss{

InstancePartStatus::InstancePartStatus()
	: refKeyframe(0)
	, refStartframe(0)
	, refEndframe(0)
	, refSpeed(0)
	, refloopNum(0)
	, infinity(false)
	, reverse(false)
	, pingpong(false)
	, independent(false)
{
	/**/
}


void InstancePartStatus::readData(int readFlags, DataArrayReader &reader, const AnimationInitialData* init)
{
	refKeyframe		= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_curKeyframe;
	refStartframe	= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_startFrame;
	refEndframe		= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_endFrame;
	refloopNum		= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_loopNum;
	refSpeed		= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readFloat() : init->instanceValue_speed;

	int loopflag	= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_loopflag;
	infinity	= (loopflag & INSTANCE_LOOP_FLAG_INFINITY);
	reverse		= (loopflag & INSTANCE_LOOP_FLAG_REVERSE);
	pingpong	= (loopflag & INSTANCE_LOOP_FLAG_PINGPONG);
	independent	= (loopflag & INSTANCE_LOOP_FLAG_INDEPENDENT);
}


} //namespace ss
