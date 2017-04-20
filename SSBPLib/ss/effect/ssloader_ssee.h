#pragma once
#include <string>
#include "sstypes.h"
#include "player/CellCache.h"
#include "effect/SsEffectElement.h"

namespace ss{


class SsEffectNode{
private:
	int					m_parentIndex;
	SsEffectNodeType	m_type;
	SsRenderBlendType	m_blendType;
	const CellRef*		m_cellRef;
public:
	std::vector<const SsEffectElementBase*> plist;

public:
	SsEffectNode(int parentIndex, SsEffectNodeType type, SsRenderBlendType blendType, const CellRef* cellRef)
		: m_parentIndex(parentIndex)
		, m_type(type)
		, m_blendType(blendType)
		, m_cellRef(cellRef)
	{}
	~SsEffectNode(){
		for(const SsEffectElementBase* eb : plist){
			delete eb;
		}
		plist.clear();
	}

	int getParentIndex() const{ return m_parentIndex; }
	SsEffectNodeType getType() const{ return m_type; }
	SsRenderBlendType getBlendType() const{ return m_blendType; }
	const CellRef* getCellRef() const{ return m_cellRef; }

	void addElement(const SsEffectElementBase* element){
		plist.push_back(element);
	}
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


	int getFPS() const{
		return m_fps;
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



} //namespace ss
