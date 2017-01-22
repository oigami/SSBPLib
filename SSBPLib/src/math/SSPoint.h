#pragma once

namespace ss {

/**
 * 座標クラス
 */
class SSPoint
{
public:
	float x;
	float y;

public:
	SSPoint(void) : x(0), y(0)
	{
	}

	SSPoint(float x, float y) : x(x), y(y)
	{
	}

	SSPoint(const SSPoint& other) : x(other.x), y(other.y)
	{
	}
/*
	SSPoint(const SSSize& size) : x(size.width), y(size.height)
	{
	}
*/
	SSPoint& operator= (const SSPoint& other)
	{
		setPoint(other.x, other.y);
		return *this;
	}
/*
	SSPoint& operator= (const SSSize& size)
	{
		setPoint(size.width, size.height);
		return *this;
	}
*/
	SSPoint operator+(const SSPoint& right) const
	{
		return SSPoint(this->x + right.x, this->y + right.y);
	}

	SSPoint operator-(const SSPoint& right) const
	{
		return SSPoint(this->x - right.x, this->y - right.y);
	}

	SSPoint operator-() const
	{
		return SSPoint(-x, -y);
	}

	SSPoint operator*(float a) const
	{
		return SSPoint(this->x * a, this->y * a);
	}

	SSPoint operator/(float a) const
	{
		SS_ASSERT_LOG(a, "SSPoint division by 0.");
		return SSPoint(this->x / a, this->y / a);
	}

	void setPoint(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	bool equals(const SSPoint& target) const
	{
		return (fabs(this->x - target.x) < FLT_EPSILON)
			&& (fabs(this->y - target.y) < FLT_EPSILON);
	}

	bool fuzzyEquals(const SSPoint& b, float var) const
	{
		if (x - var <= b.x && b.x <= x + var)
			if (y - var <= b.y && b.y <= y + var)
				return true;
		return false;
	}
/*
	float getAngle(const SSPoint& other) const
	{
		SSPoint a2 = normalize();
		SSPoint b2 = other.normalize();
		float angle = atan2f(a2.cross(b2), a2.dot(b2));
		if (fabs(angle) < FLT_EPSILON) return 0.f;
		return angle;
	}

	SSPoint rotateByAngle(const SSPoint& pivot, float angle) const
	{
		return pivot + (*this - pivot).rotate(SSPoint::forAngle(angle));
	}
*/

	inline float getLength() const {
		return sqrtf(x*x + y*y);
	};
	inline float getLengthSq() const {
		return dot(*this); //x*x + y*y;
	};
	inline float getDistanceSq(const SSPoint& other) const {
		return (*this - other).getLengthSq();
	};
	inline float getDistance(const SSPoint& other) const {
		return (*this - other).getLength();
	};
	inline float getAngle() const {
		return atan2f(y, x);
	};
	float getAngle(const SSPoint& other) const;
	inline float dot(const SSPoint& other) const {
		return x*other.x + y*other.y;
	};
	inline float cross(const SSPoint& other) const {
		return x*other.y - y*other.x;
	};
	inline SSPoint getPerp() const {
		return SSPoint(-y, x);
	};
	inline SSPoint getRPerp() const {
		return SSPoint(y, -x);
	};
	inline SSPoint project(const SSPoint& other) const {
		return other * (dot(other) / other.dot(other));
	};
	inline SSPoint rotate(const SSPoint& other) const {
		return SSPoint(x*other.x - y*other.y, x*other.y + y*other.x);
	};
	inline SSPoint unrotate(const SSPoint& other) const {
		return SSPoint(x*other.x + y*other.y, y*other.x - x*other.y);
	};
	inline SSPoint normalize() const {
		float length = getLength();
		if (length == 0.) return SSPoint(1.f, 0);
		return *this / getLength();
	};
	inline SSPoint lerp(const SSPoint& other, float alpha) const {
		return *this * (1.f - alpha) + other * alpha;
	};
	SSPoint rotateByAngle(const SSPoint& pivot, float angle) const;

	static inline SSPoint forAngle(const float a)
	{
		return SSPoint(cosf(a), sinf(a));
	}
};

	

} //namespace ss
