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

	int			m_flags;		/// 含まれるデータの種類
	int			m_partIndex;	/// パーツインデックス
	int			m_integer;		/// 整数データ
	int			m_rect[4];		/// 矩形データ[Left, Top, Right, Bottom]
	int			m_point[2];		/// 座標データ[X, Y]
	const char*	m_str;			/// 文字列データ (zero terminated)
	int			m_strSize;		/// 文字列のサイズ (byte count)

	
	//readerを介してデータを読み取る
	void readData(DataArrayReader& reader, const ToPointer& ptr);
private:
	void init();
};


} //namespace ss
