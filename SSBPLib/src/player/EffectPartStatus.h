#pragma once

namespace ss{
class DataArrayReader;
struct AnimationInitialData;

class EffectPartStatus{	//SsEffectAttr相当
public:
	EffectPartStatus();

	//readerを介してデータを読み取る
	void readData(int readFlags, DataArrayReader &reader, const AnimationInitialData* init);


	int		m_curKeyframe;	//エフェクトが配置されたキーフレーム	/*effectValue_curKeyframe*/
	int		m_startTime;	//再生開始フレーム	/*effectValue_startTime*/
	float	m_speed;		//再生スピード		/*effectValue_speed*/
	
	//再生制御フラグ	/*effectValue_loopflag*/
	bool	m_independent;	//独立動作
};


} //namespace ss
