#include "UserData.h"
#include "DataArrayReader.h"
#include "ToPointer.h"

namespace ss{

//データ読み取り
void UserData::readData(DataArrayReader& reader, const ToPointer& ptr)
{
	int flags = reader.readU16();
	m_partIndex = reader.readU16();

	m_flags = 0;

	if(flags & UserData::FLAG_INTEGER)
	{
		m_flags |= UserData::FLAG_INTEGER;
		m_integer = reader.readS32();
	}
	else
	{
		m_integer = 0;
	}

	if(flags & UserData::FLAG_RECT)
	{
		m_flags |= UserData::FLAG_RECT;
		m_rect[0] = reader.readS32();
		m_rect[1] = reader.readS32();
		m_rect[2] = reader.readS32();
		m_rect[3] = reader.readS32();
	}
	else
	{
		m_rect[0] =
			m_rect[1] =
			m_rect[2] =
			m_rect[3] = 0;
	}

	if(flags & UserData::FLAG_POINT)
	{
		m_flags |= UserData::FLAG_POINT;
		m_point[0] = reader.readS32();
		m_point[1] = reader.readS32();
	}
	else
	{
		m_point[0] =
			m_point[1] = 0;
	}

	if(flags & UserData::FLAG_STRING)
	{
		m_flags |= UserData::FLAG_STRING;
		int size = reader.readU16();
		ss_offset offset = reader.readOffset();
		const char* str = ptr.toString(offset);
		m_str = str;
		m_strSize = size;
	}
	else
	{
		m_str = 0;
		m_strSize = 0;
	}
}



} //namespace ss
