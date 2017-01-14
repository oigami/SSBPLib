#pragma once
#include "SS5PlayerData.h"

namespace ss{

/**
 * ProjectDataから各データへのアクセサ
 *  特定の型へのアクセスしかしないので、読みやすさのためにもアクセス方法を提供しておく
 */
class ToPointer{
public:
	explicit ToPointer(const ProjectData* data)
		: m_base(static_cast<const uint8_t*>(static_cast<const void*>(data))) {}
	

	const Cell* toCells(const ProjectData* adr) const{
		return toClass<Cell>( adr->cells );					//return static_cast<const Cell*>(getAddress( adr->cells ));
	}
	const CellMap* toCellMap(const Cell* adr) const{
		return toClass<CellMap>( adr->cellMap );
	}
	
	const AnimePackData* toAnimePackDatas(const ProjectData* adr) const{
		return toClass<AnimePackData>( adr->animePacks );	//return static_cast<const AnimePackData*>(getAddress( adr->animePacks));
	}
	const PartData* toPartDatas(const AnimePackData* adr) const{
		return toClass<PartData>( adr->parts );
	}
	const AnimationData* toAnimationDatas(const AnimePackData* adr) const{
		return toClass<AnimationData>( adr->animations );
	}
	const AnimationInitialData* toAnimationInitialDatas(const AnimationData* adr) const{
		return toClass<AnimationInitialData>( adr->defaultData );
	}

	const EffectFile* toEffectFiles(const ProjectData* adr) const{
		return toClass<EffectFile>( adr->effectFileList );
	}
	const EffectNode* toEffectNodes(const EffectFile* adr) const{
		return toClass<EffectNode>( adr->effectNode );
	}


	//ほかのアクセスはほとんどconst char*なのでこれはoffsetを取るようにしとく
	const char* toString(ss_offset offset) const{
		return toClass<char>( offset );
	}
		
		
//private:	
	const void* operator()(ss_offset offset) const{
		return getAddress(offset);
	}
	
private:
	//キャストのテンプレート化
	template<class T>
	const T* toClass(ss_offset offset) const{
		return static_cast<const T*>(getAddress( offset ));
	}

	const void* getAddress(ss_offset offset) const{
		return (m_base + offset);
	}


	const uint8_t*	m_base;			//ProjectDataの先頭アドレス
};


} //namespace ss
