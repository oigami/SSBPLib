#pragma once
#include <string>

namespace ss{
class DataArrayReader;
class ToPointer;

/**
 * UserData
 */
struct UserData{
	enum {
		FLAG_INTEGER = 1 << 0,
		FLAG_RECT = 1 << 1,
		FLAG_POINT = 1 << 2,
		FLAG_STRING = 1 << 3
	};

	const char*	m_partName;		/// Part name
	int			m_frameNo;		/// Frame no

	int			m_flags;		/// 含まれるデータの種類
	int			m_partIndex;	/// パーツインデックス
	int			m_integer;		/// 整数データ
	int			m_rect[4];		/// 矩形データ[Left, Top, Right, Bottom]
	int			m_point[2];		/// 座標データ[X, Y]
	const char*	m_str;			/// 文字列データ (zero terminated)
	int			m_strSize;		/// 文字列のサイズ (byte count)

	
	//readerを介してデータを読み取る
	void readData(DataArrayReader& reader, const ToPointer& ptr);
};


/**
 * LabelData
 */
struct LabelData
{
	std::string	str;			/// String (zero terminated)
	int			strSize;		/// String size (byte count)
	int			frameNo;		/// Frame no
};

//インスタンスデータ
struct Instance
{
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
};

} //namespace ss
