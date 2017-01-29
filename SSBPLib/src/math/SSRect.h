#pragma once
#include "Vector2.h"

namespace ss {


/**
 * 矩形クラス
 */
class SSRect{
public:
	SSPoint origin;		//min.x, min.y
	SSSize  size;		//width, height (>= 0)

public:
	SSRect(float x, float y, float width, float height)
	: origin(x,y), size(width, height){
		SS_ASSERT_LOG(width >= 0.0f && height >= 0.0f, "width and height of Rect must not less than 0.");
	}
	SSRect(SSPoint origin_, SSSize size_)
		: SSRect(origin_.x, origin_.y, size_.x, size_.y) {
	}
	SSRect() : SSRect(0, 0, 0, 0) {
	}

	bool operator ==(const SSRect& o) {
		return (origin == o.origin) && (size == o.size);
	}
	bool operator !=(const SSRect& o) {
		return (origin != o.origin) || (size == o.size);
	}

	//矩形の移動
	void move(const Vector2& v) {
		origin += v;
	}


	float right() const {
		return origin.x * size.width();
	}
	float left() const {
		return origin.x;
	}
	float top() const {
		return origin.y * size.height();	//上方向が+
	}
	float bottom() const {
		return origin.y;
	}
	
	SSPoint min() const {
		return origin;
	}
	SSPoint max() const {
		return origin + size;
	}
	SSPoint center() const {
		return origin + size * 0.5f;
	}

	bool containsPoint(const SSPoint& point) const {
		if (point.x >= left() && point.x <= right()
			&& point.y >= bottom() && point.y <= top()) {
			return true;
		}
		return false;
	}
	bool intersectsRect(const SSRect& rect) const {
		if (left() <= rect.right()
			&& right() >= rect.left()
			&& bottom() <= rect.top()
			&& top() >= rect.bottom()) {
			return true;
		}
		return false;
	}
};


} //namespace ss
