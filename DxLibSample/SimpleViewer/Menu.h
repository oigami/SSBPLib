#pragma once

#include <memory>
#include <iostream>
#include <vector>
#include <list>
#include <string>

namespace ss{
	class SS5Player;
}


class MenuInterface{
public:
	virtual ~MenuInterface(){}
	virtual void action(int up, int down, int left, int right, int enter, int cancel) = 0;
	virtual void draw(std::ostream &oss) = 0;
};


template<class T>
class MenuBase : public MenuInterface{
public:
	MenuBase(T* t) :m_context(t){}
	virtual ~MenuBase(){}

	T* get(){ return m_context; }
	const T* get() const{ return m_context; }
private:
	T* m_context;
};

using Base = MenuBase<ss::SS5Player>;


//メニューのセレクタ
class MenuRoot : public Base{
public:
	MenuRoot(ss::SS5Player* p);
	~MenuRoot();

	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

private:
	std::vector<Base*> m_childMenu;
	int m_activeIndex;
};


//--
struct AnimeChanger : public Base{
	AnimeChanger(ss::SS5Player* p) : Base(p), m_select(0){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;
	
	std::string m_animeName;
	int m_select;
};

struct FrameChanger : public Base{
	FrameChanger(ss::SS5Player* p) : Base(p), m_frame(0){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	int m_frame;
};

struct PositionChanger : public Base{
	enum class XYZ{ X, Y/*, Z*/ };
	PositionChanger(ss::SS5Player* p, XYZ xyz, int pos) : Base(p), m_xyz(xyz), m_position(pos), INIT_POS(pos){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	XYZ m_xyz;
	int m_position;
	const int INIT_POS;
};

struct RotationChanger : public Base{
	enum class XYZ{ X, Y, Z };
	RotationChanger(ss::SS5Player* p, XYZ xyz) : Base(p), m_xyz(xyz), m_rotation(0){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	XYZ m_xyz;
	int m_rotation;
};

struct ScaleChanger : public Base{
	enum class XYZ{ X, Y/*, Z*/ };
	ScaleChanger(ss::SS5Player* p, XYZ xyz) : Base(p), m_xyz(xyz), m_scale(0.5){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	XYZ m_xyz;
	double m_scale;
};

struct AlphaChanger : public Base{
	AlphaChanger(ss::SS5Player* p) : Base(p), m_alpha(1.0){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	double m_alpha;
};

