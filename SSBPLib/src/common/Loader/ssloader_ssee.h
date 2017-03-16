#ifndef __SSLOADER_SSEE__
#define __SSLOADER_SSEE__

#include <string>
#include "sstypes.h"

#include "SsEffectBehavior.h"

namespace ss
{


class SsEffectNode{
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
