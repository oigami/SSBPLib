#pragma once
#include "Vector2.h"

namespace ss {


/**
 * 矩形クラス
 */
class SSRect{
private:
	SSPoint m_origin;	//min.x, min.y
	SSSize  m_size;		//width, height (>= 0)

public:
	SSRect(float x, float y, float width, float height)
	: m_origin(x,y), m_size(width, height){
		SS_ASSERT_LOG(width >= 0.0f && height >= 0.0f, "width and height of Rect must not less than 0.");
	}
	SSRect(SSPoint origin, SSSize size)
		: SSRect(origin.x, origin.y, size.x, size.y) {
	}
	SSRect() : SSRect(0, 0, 0, 0) {
	}

	bool operator ==(const SSRect& o) const{
		return (m_origin == o.m_origin) && (m_size == o.m_size);
	}
	bool operator !=(const SSRect& o) const{
		return (m_origin != o.m_origin) || (m_size == o.m_size);
	}

	//矩形の移動
	void move(const Vector2& v) {
		m_origin += v;
	}


	float right() const {
		return m_origin.x + m_size.width();
	}
	float left() const {
		return m_origin.x;
	}
	float top() const {
		return m_origin.y + m_size.height();	//上方向が+
	}
	float bottom() const {
		return m_origin.y;
	}
	float width() const{
		return m_size.width();
	}
	float height() const{
		return m_size.height();
	}
	
	SSPoint min() const {
		return m_origin;
	}
	SSPoint max() const {
		return m_origin + m_size;
	}
	SSPoint center() const {
		return m_origin + m_size * 0.5f;
	}
	SSSize size() const{
		return m_size;
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
