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


int InstancePartStatus::getFrame(int time) const
{
	//このインスタンスが配置されたキーフレーム（絶対時間）
	int	selfTopKeyframe = this->refKeyframe;


	int	reftime = (int)((float)(time - selfTopKeyframe) * this->refSpeed); //開始から現在の経過時間
	if(reftime < 0){ return time; }							//そもそも生存時間に存在していない
	if(selfTopKeyframe > time){ return time; }

	int inst_scale = (this->refEndframe - this->refStartframe) + 1; //インスタンスの尺


	//尺が０もしくはマイナス（あり得ない
	if(inst_scale <= 0){ return time; }
	int	nowloop = (reftime / inst_scale);	//現在までのループ数

	int checkloopnum = this->refloopNum;

	//pingpongの場合では２倍にする
	if(this->pingpong) checkloopnum = checkloopnum * 2;

	//無限ループで無い時にループ数をチェック
	if(!this->infinity){   //無限フラグが有効な場合はチェックせず
		if(nowloop >= checkloopnum){
			reftime = inst_scale - 1;
			nowloop = checkloopnum - 1;
		}
	}

	int temp_frame = reftime % inst_scale;  //ループを加味しないインスタンスアニメ内のフレーム

											//参照位置を決める
											//現在の再生フレームの計算
	int _time = 0;
	bool _reverse = this->reverse;
	if(this->pingpong && (nowloop % 2 == 1)){
		if(this->reverse){
			_reverse = false;//反転
		}
		else{
			_reverse = true;//反転
		}
	}

	if(_reverse){
		//リバースの時
		_time = this->refEndframe - temp_frame;
	}
	else{
		//通常時
		_time = temp_frame + this->refStartframe;
	}
}


} //namespace ss
