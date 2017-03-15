#ifndef __SSLOADER_SSEE__
#define __SSLOADER_SSEE__

#include <string>
#include "sstypes.h"

#include "SsEffectBehavior.h"

namespace ss
{


class SimpleTree
{
#if 0
private:
	std::list<SimpleTree*> children;
	
public:
	SimpleTree(){}
	virtual ~SimpleTree(){
		for(SimpleTree* c : children){
			delete c;	//複数登録されてるものがいるとまずいが、そのような使われ方はされてない模様
		}
		children.clear();
	}

	std::list<SimpleTree*>& getChildren(){ return children; }
	void addChildEnd(SimpleTree* c){ children.push_back(c); }
	

#else

private:
	SimpleTree *parent;
	SimpleTree *childTop;
	SimpleTree *prev;
	SimpleTree *next;

public:
	SimpleTree()
		: parent(0), childTop(0), prev(0), next(0)
	{}
	virtual ~SimpleTree()
	{
		destroy();
	}

	void	addChildEnd(SimpleTree* c){
		if (childTop == 0)
		{
			childTop = c;
		}else{
			childTop->addSiblingEnd(c);
		}
		c->parent = this;
	}

private:
	void	addSiblingEnd(SimpleTree* c)
	{
		if (next == 0)
		{
			c->prev = this;
			next = c;
		}else{
			next->addSiblingEnd(c);
		}

		c->parent = this->parent;
	}

	void destroysub(SimpleTree *t)
	{
		if (t->childTop)
		{
			destroysub(t->childTop);
		}
		if (t->next)
		{
			destroysub(t->next);
		}

		t->childTop = 0;
		t->next = 0;
		t->prev = 0;
		delete t;
	}
	void destroy()
	{
		if (this->childTop)
			destroysub(this->childTop);
	}
#endif
};






class SsEffectNode : public SimpleTree
{
private:
	int						m_parentIndex;
	SsEffectNodeType::_enum	m_type;

	SsEffectBehavior		m_behavior;

public:
	SsEffectNode(int parentIndex, SsEffectNodeType::_enum type, const SsEffectBehavior& behavior)
		: m_parentIndex(parentIndex)
		, m_type(type)
		, m_behavior(behavior)
	{}
	~SsEffectNode(){}

	int getParentIndex() const{ return m_parentIndex; }
	SsEffectNodeType::_enum	GetType() const{ return m_type; }
	SsEffectBehavior*	GetMyBehavior(){ return &m_behavior; }

};


class SsEffectModel
{
private:

public:
	SsEffectNode* root;
	std::vector<SsEffectNode*> nodeList;
	int			lockRandSeed; 	 // ランダムシード固定値
	bool    	isLockRandSeed;  // ランダムシードを固定するか否か
	int			fps;             //
	std::string	effectName;
	int			layoutScaleX;
	int			layoutScaleY;

public:
	SsEffectModel() : root(0)
	{}

	virtual ~SsEffectModel(){
		if (root)
		{
			delete root;
		}
		root = 0;
	}

	//アクセス
	SsEffectNode* GetRoot(){ return root; }

	const std::vector<SsEffectNode*>& getNodeList()
	{
		return nodeList;
	}
};



};

#endif
