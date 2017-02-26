#pragma once


namespace ss{
class DataArrayReader;
struct AnimationInitialData;

/** インスタンスパーツの状態 */
struct InstancePartStatus{

	int		m_refKeyframe;		//このインスタンスが配置されたキーフレーム /*instanceValue_curKeyframe*/
	int		m_refStartframe;	//開始フレーム	/*instanceValue_startFrame*/
	int		m_refEndframe;		//終了フレーム	/*instanceValue_endFrame*/
	int		m_refloopNum;		//ループ回数	/*instanceValue_loopNum*/
	float	m_refSpeed;			//再生速度		/*instanceValue_speed*/

	/*instanceValue_loopflag*/
	bool	m_infinity;			//無限ループ
	bool	m_reverse;			//逆再生
	bool	m_pingpong;			//往復
	bool	m_independent;		//独立動作


	InstancePartStatus();

	//readerを介してデータを読み取る
	void readData(int readFlags, DataArrayReader &reader, const AnimationInitialData* init);


	//親の時間を元に、インスタンスパーツのフレームを計算する
	int getFrame(int frame) const;

#if 0
private:
	int endFrame() const;	//フラグ情報をみて終了フレームを返す
#endif
};


} //namespace ss
