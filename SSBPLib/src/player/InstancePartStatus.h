#pragma once


namespace ss{
class DataArrayReader;

/** インスタンスパーツの状態 */
struct InstancePartStatus
{
	//int m_refKeyframe;	//このインスタンスが配置されたキーフレーム（絶対時間）
	int			refStartframe;		//開始フレーム
	int			refEndframe;		//終了フレーム
	float		refSpeed;			//再生速度
	int			refloopNum;			//ループ回数

	bool		infinity;			//無限ループ
	bool		reverse;			//逆再選
	bool		pingpong;			//往復
	bool		independent;		//独立動作
	void clear(void)
	{
		refStartframe = 0;			//開始フレーム
		refEndframe = 1;			//終了フレーム
		refSpeed = 1;				//再生速度
		refloopNum = 1;				//ループ回数
		infinity = false;			//無限ループ
		reverse = false;			//逆再選
		pingpong = false;			//往復
		independent = false;		//独立動作
	}


#if 0
	InstancePartStatus();

	//readerを介してデータを読み取る
	void readInstancePartStatus(DataArrayReader &readerm, int readFlags);

	//読み取り後になんか計算してる部分を抜き出した
	//親の時間を元に、インスタンスパーツのフレームを計算する
	int getFrame(int frame) const;

private:
	int endFrame() const;	//フラグ情報をみて終了フレームを返す
#endif
};


} //namespace ss
