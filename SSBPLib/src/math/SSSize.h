#pragma once

#include "Vector2.h"

#if 0
#include "SSPoint.h"
#include "player/Util.h"



namespace ss {

/**
 * サイズクラス memo:SSPoint使えばいい気がする
 */
class SSSize
{
public:
	float width;
	float height;

public:
	SSSize(void) : width(0), height(0)
	{
	}

	SSSize(float width, float height) : width(width), height(height)
	{
	}

	SSSize(const SSSize& other) : width(other.width), height(other.height)
	{
	}

	SSSize(const SSPoint& point) : width(point.x), height(point.y)
	{
	}

	SSSize& operator= (const SSSize& other)
	{
		setSize(other.width, other.height);
		return *this;
	}

	SSSize& operator= (const SSPoint& point)
	{
		setSize(point.x, point.y);
		return *this;
	}

	SSSize operator+(const SSSize& right) const
	{
		return SSSize(this->width + right.width, this->height + right.height);
	}

	SSSize operator-(const SSSize& right) const
	{
		return SSSize(this->width - right.width, this->height - right.height);
	}

	SSSize operator*(float a) const
	{
		return SSSize(this->width * a, this->height * a);
	}

	SSSize operator/(float a) const
	{
		SS_ASSERT_LOG(a, "SSSize division by 0.");
		return SSSize(this->width / a, this->height / a);
	}

	void setSize(float width, float height)
	{
		this->width = width;
		this->height = height;
	}
#if 0
	bool equals(const SSSize& target) const
	{
		return (fabs(this->width - target.width)  < FLT_EPSILON)
			&& (fabs(this->height - target.height) < FLT_EPSILON);
	}
#endif
};


} //namespace ss
#endif
