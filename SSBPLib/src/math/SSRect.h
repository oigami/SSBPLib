#pragma once
#include "SSPoint.h"
#include "SSSize.h"

namespace ss {


/**
* 矩形クラス	memo:tbrl top,bottom,right,leftの形式にしたい
*/
class SSRect
{
public:
	SSPoint origin;
	SSSize  size;

public:

	SSRect(void)
	{
		setRect(0.0f, 0.0f, 0.0f, 0.0f);
	}

	SSRect(float x, float y, float width, float height)
	{
		setRect(x, y, width, height);
	}

	SSRect(const SSRect& other)
	{
		setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
	}

	SSRect& operator= (const SSRect& other)
	{
		setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
		return *this;
	}

	void setRect(float x, float y, float width, float height)
	{
		// CGRect can support width<0 or height<0
		SS_ASSERT_LOG(width >= 0.0f && height >= 0.0f, "width and height of Rect must not less than 0.");

		origin.x = x;
		origin.y = y;

		size.width = width;
		size.height = height;
	}

	bool equals(const SSRect& rect) const
	{
		return (origin.equals(rect.origin) &&
			size.equals(rect.size));
	}

	float getMaxX() const
	{
		return (float)(origin.x + size.width);
	}

	float getMidX() const
	{
		return (float)(origin.x + size.width / 2.0);
	}

	float getMinX() const
	{
		return origin.x;
	}

	float getMaxY() const
	{
		return origin.y + size.height;
	}

	float getMidY() const
	{
		return (float)(origin.y + size.height / 2.0);
	}

	float getMinY() const
	{
		return origin.y;
	}

	bool containsPoint(const SSPoint& point) const
	{
		bool bRet = false;

		if (point.x >= getMinX() && point.x <= getMaxX()
			&& point.y >= getMinY() && point.y <= getMaxY())
		{
			bRet = true;
		}

		return bRet;
	}

	bool intersectsRect(const SSRect& rect) const
	{
		return !(getMaxX() < rect.getMinX() ||
			rect.getMaxX() <      getMinX() ||
			getMaxY() < rect.getMinY() ||
			rect.getMaxY() <      getMinY());
	}

};


} //namespace ss
