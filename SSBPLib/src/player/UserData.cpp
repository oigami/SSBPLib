#include "UserData.h"
#include "DataArrayReader.h"
#include "ToPointer.h"

namespace ss{

//データ読み取り
void UserData::readData(DataArrayReader& reader, const ToPointer& ptr)
{
	int flags = reader.readU16();
	this->partIndex = reader.readU16();

	this->flags = 0;

	if(flags & UserData::FLAG_INTEGER)
	{
		this->flags |= UserData::FLAG_INTEGER;
		this->integer = reader.readS32();
	}
	else
	{
		this->integer = 0;
	}

	if(flags & UserData::FLAG_RECT)
	{
		this->flags |= UserData::FLAG_RECT;
		this->rect[0] = reader.readS32();
		this->rect[1] = reader.readS32();
		this->rect[2] = reader.readS32();
		this->rect[3] = reader.readS32();
	}
	else
	{
		this->rect[0] =
			this->rect[1] =
			this->rect[2] =
			this->rect[3] = 0;
	}

	if(flags & UserData::FLAG_POINT)
	{
		this->flags |= UserData::FLAG_POINT;
		this->point[0] = reader.readS32();
		this->point[1] = reader.readS32();
	}
	else
	{
		this->point[0] =
			this->point[1] = 0;
	}

	if(flags & UserData::FLAG_STRING)
	{
		this->flags |= UserData::FLAG_STRING;
		int size = reader.readU16();
		ss_offset offset = reader.readOffset();
		const char* str = ptr.toString(offset);
		this->str = str;
		this->strSize = size;
	}
	else
	{
		this->str = 0;
		this->strSize = 0;
	}
}



} //namespace ss
