#pragma once

namespace ss{
class DataArrayReader;
struct AnimationInitialData;

class EffectPartStatus{	//SsEffectAttr相当
public:
	EffectPartStatus();

	//readerを介してデータを読み取る
	void readData(int readFlags, DataArrayReader &reader, const AnimationInitialData* init);

	/**
	 * 再生可能な時間ならtrue
	 * @param frame	親のフレーム
	 */
	bool isValidFrame(int frame) const;
	
	/** 親のフレームを元に、エフェクトのフレームを計算する */
	float getFrame(int frame) const;


	int		m_curKeyframe;	//キーフレーム		/*effectValue_curKeyframe*/
	int		m_startTime;	//再生開始フレーム	/*effectValue_startTime*/
	float	m_speed;		//再生スピード		/*effectValue_speed*/
	
	//再生制御フラグ	/*effectValue_loopflag*/
	bool	m_independent;	//独立動作
};


} //namespace ss
