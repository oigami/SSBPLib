#ifndef __SSLOADER_SSEE__
#define __SSLOADER_SSEE__

#include <string>
#include "sstypes.h"

#include "SsEffectBehavior.h"

namespace ss
{


class SsEffectNode{
private:
	int				 m_parentIndex;
	SsEffectNodeType m_type;

	SsEffectBehavior m_behavior;

public:
	SsEffectNode(int parentIndex, SsEffectNodeType type, const SsEffectBehavior& behavior)
		: m_parentIndex(parentIndex)
		, m_type(type)
		, m_behavior(behavior)
	{}
	~SsEffectNode(){
		//ひとまずEffectCacheでのdeleteをこちらに移動。deleteはSsEffectBehaviorがやるべきなので後で修正する
		for(const SsEffectElementBase* eb : GetMyBehavior()->plist){
			delete eb;
		}
		m_behavior.plist.clear();
	}

	int getParentIndex() const{ return m_parentIndex; }
	SsEffectNodeType GetType() const{ return m_type; }
	const SsEffectBehavior*	GetMyBehavior() const{ return &m_behavior; }

};


class SsEffectModel{
private:
	std::string	m_effectName;		//エフェクトファイル名
	int			m_fps;				//FPS
	bool    	m_isLockRandSeed;	//乱数を固定するかどうか
	int			m_lockRandSeed; 	//固定する場合の乱数の種
	Vector2		m_layoutScale;		//レイアウトスケール
	std::vector<const SsEffectNode*> m_nodeList;

public:
	SsEffectModel(std::string effectName, int fps, bool isLockRandSeed, int lockRandSeed, int layoutScaleX, int layoutScaleY)
		: m_effectName(effectName)
		, m_fps(fps)
		, m_isLockRandSeed(isLockRandSeed)
		, m_lockRandSeed(lockRandSeed)
		, m_layoutScale(layoutScaleX, layoutScaleY)
	{}

	~SsEffectModel(){
		for(const SsEffectNode* node : m_nodeList){
			delete node;
		}
		m_nodeList.clear();
	}

	bool isLockRandSeed() const{
		return m_isLockRandSeed;
	}
	int lockRandSeed() const{
		return m_lockRandSeed;
	}
	const Vector2& layoutScale() const{
		return m_layoutScale;
	}

	const std::vector<const SsEffectNode*>& getNodeList() const{
		return m_nodeList;
	}

	void addNode(const SsEffectNode* node){
		m_nodeList.push_back(node);
	}
};



};

#endif
