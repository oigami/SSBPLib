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
	int	reftime = static_cast<int>((frame - m_refKeyframe) * m_refSpeed);	//開始から現在の経過時間
	if(reftime < 0){ return frame; }										//そもそも生存時間に存在していない
	if(m_refKeyframe > frame){ return frame; }		//memo:このあたりの制限特に要らない気がする。時間がエラー範囲のときに返すべきフレームもないと思うので。

	int inst_scale = (m_refEndframe - m_refStartframe) + 1; //インスタンスの尺
	if(inst_scale <= 0){ return frame; }					//尺が０もしくはマイナス（あり得ない
	
	int	nowloop = (reftime / inst_scale);	//現在までのループ数


	//無限ループで無い時 --> 回数制限があるので終端チェックをする
	if(!m_infinity){
		int checkloopnum = m_refloopNum;		//ループ終了数
		if(m_pingpong){ checkloopnum *= 2; }	//pingpongの場合では２倍にする
	
		if(nowloop >= checkloopnum){
			reftime = inst_scale - 1;
			nowloop = checkloopnum - 1;
			/**
			 * memo:ここを通過したときの_timeは、計算上こうなる。
			 * リバース時:
			 * m_refEndframe - temp_frame;
			 * --> m_refEndframe - (reftime % inst_scale)
			 * --> m_refEndframe - ((inst_scale - 1) % inst_scale)
			 * --> m_refEndframe - ((m_refEndframe - m_refStartframe + 1 - 1) % (m_refEndframe - m_refStartframe + 1))
			 * --> m_refEndframe - ((m_refEndframe - m_refStartframe) % (m_refEndframe - m_refStartframe + 1))
			 * --> m_refEndframe - (m_refEndframe - m_refStartframe)
			 * --> m_refStartframe
			 * 
			 * 通常時
			 * temp_frame + m_refStartframe
			 * --> m_refStartFrame + temp_frame
			 * --> m_refStartFrame + (m_refEndframe - m_refStartframe)
			 * --> m_refEndframe
			 *
			 * ということなので、結局reverseに応じてm_refStartFrameかm_refEndframeを返せば良いという事になる
			 */
		}
	}

	int temp_frame = reftime % inst_scale;  //ループを加味しないインスタンスアニメ内のフレーム

	//現在の再生フレームの計算
	bool isReverse = m_reverse;
	if(m_pingpong && (nowloop % 2 == 1)){
		if(m_reverse){
			isReverse = false;//反転
		}
		else{
			isReverse = true;//反転
		}
	}

	if(isReverse){
		return m_refEndframe - temp_frame;	//リバース時
	}
	return m_refStartframe + temp_frame;	//通常時
}


} //namespace ss
