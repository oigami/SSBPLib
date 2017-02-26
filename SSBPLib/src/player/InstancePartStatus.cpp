#include "InstancePartStatus.h"
#include "DataArrayReader.h"
#include "PlayerDef.h"
#include "SS5PlayerData.h"

namespace ss{

InstancePartStatus::InstancePartStatus()
	: m_refKeyframe(0)
	, m_refStartframe(0)
	, m_refEndframe(0)
	, m_refSpeed(0)
	, m_refloopNum(0)
	, m_infinity(false)
	, m_reverse(false)
	, m_pingpong(false)
	, m_independent(false)
{
	/**/
}


void InstancePartStatus::readData(int readFlags, DataArrayReader &reader, const AnimationInitialData* init)
{
	m_refKeyframe	= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_curKeyframe;
	m_refStartframe	= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_startFrame;
	m_refEndframe	= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_endFrame;
	m_refloopNum	= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_loopNum;
	m_refSpeed		= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readFloat() : init->instanceValue_speed;

	int loopflag	= readFlags & PART_FLAG_INSTANCE_KEYFRAME ? reader.readS32() : init->instanceValue_loopflag;
	m_infinity		= (loopflag & INSTANCE_LOOP_FLAG_INFINITY);
	m_reverse		= (loopflag & INSTANCE_LOOP_FLAG_REVERSE);
	m_pingpong		= (loopflag & INSTANCE_LOOP_FLAG_PINGPONG);
	m_independent	= (loopflag & INSTANCE_LOOP_FLAG_INDEPENDENT);
}


int InstancePartStatus::getFrame(int frame) const
{
	//このインスタンスが配置されたキーフレーム（絶対時間）
	int	selfTopKeyframe = m_refKeyframe;


	int	reftime = (int)((float)(frame - selfTopKeyframe) * m_refSpeed); //開始から現在の経過時間
	if(reftime < 0){ return frame; }							//そもそも生存時間に存在していない
	if(selfTopKeyframe > frame){ return frame; }

	int inst_scale = (m_refEndframe - m_refStartframe) + 1; //インスタンスの尺


	//尺が０もしくはマイナス（あり得ない
	if(inst_scale <= 0){ return frame; }
	int	nowloop = (reftime / inst_scale);	//現在までのループ数

	int checkloopnum = m_refloopNum;

	//pingpongの場合では２倍にする
	if(m_pingpong) checkloopnum = checkloopnum * 2;

	//無限ループで無い時にループ数をチェック
	if(!m_infinity){   //無限フラグが有効な場合はチェックせず
		if(nowloop >= checkloopnum){
			reftime = inst_scale - 1;
			nowloop = checkloopnum - 1;
		}
	}

	int temp_frame = reftime % inst_scale;  //ループを加味しないインスタンスアニメ内のフレーム

											//参照位置を決める
											//現在の再生フレームの計算
	int _time = 0;
	bool _reverse = m_reverse;
	if(m_pingpong && (nowloop % 2 == 1)){
		if(m_reverse){
			_reverse = false;//反転
		}
		else{
			_reverse = true;//反転
		}
	}

	if(_reverse){
		//リバースの時
		_time = m_refEndframe - temp_frame;
	}
	else{
		//通常時
		_time = temp_frame + m_refStartframe;
	}
}


} //namespace ss
