#ifndef __SSLOADER_SSEE__
#define __SSLOADER_SSEE__

#include <string>
#include "sstypes.h"

#include "SsEffectBehavior.h"

namespace ss
{


class SimpleTree
{
private:
	SimpleTree *parent;
	SimpleTree *childTop;
	SimpleTree *prev;
	SimpleTree *next;
public:
	SimpleTree* getChildTop(){ return childTop; }
	SimpleTree* getNext(){ return next; }

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
};






class SsEffectNode : public SimpleTree
{
public:
	int						arrayIndex;
	int						parentIndex;
	SsEffectNodeType::_enum	type;
	bool					visible;

	SsEffectBehavior		behavior;

public:
	SsEffectNode() :
		arrayIndex(0), parentIndex(0),
		type(SsEffectNodeType::invalid)
	{}
	~SsEffectNode(){}

	SsEffectNodeType::_enum	GetType(){ return type; }

	SsEffectBehavior*	GetMyBehavior(){ return &behavior; }

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
