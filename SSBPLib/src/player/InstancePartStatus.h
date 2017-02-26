#pragma once


namespace ss{
class DataArrayReader;
struct AnimationInitialData;

/** インスタンスパーツの状態 */
struct InstancePartStatus{

	int			refKeyframe;		//このインスタンスが配置されたキーフレーム /*instanceValue_curKeyframe*/
	int			refStartframe;		//開始フレーム	/*instanceValue_startFrame*/
	int			refEndframe;		//終了フレーム	/*instanceValue_endFrame*/
	int			refloopNum;			//ループ回数	/*instanceValue_loopNum*/
	float		refSpeed;			//再生速度		/*instanceValue_speed*/

	/*instanceValue_loopflag*/
	bool		infinity;			//無限ループ
	bool		reverse;			//逆再生
	bool		pingpong;			//往復
	bool		independent;		//独立動作


	InstancePartStatus();

	//readerを介してデータを読み取る
	void readData(int readFlags, DataArrayReader &reader, const AnimationInitialData* init);


	//読み取り後になんか計算してる部分を抜き出した
	//親の時間を元に、インスタンスパーツのフレームを計算する
	int getFrame(int time) const;

#if 0
private:
	int endFrame() const;	//フラグ情報をみて終了フレームを返す
#endif
};


} //namespace ss
