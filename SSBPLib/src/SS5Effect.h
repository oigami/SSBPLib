#pragma once
#include <vector>
#include "SS5PlayerTypes.h"
#include "effect/sstypes.h"
#include "math/Matrix.h"

namespace ss{
class SS5EventListener;
class SsEffectModel;
class SsEffectNode;
class SsEffectEmitter;
struct particleDrawData;
class CustomSprite;
struct CellRef;
class ResourceSet;


class SsEffectRenderV2{
private:
	SS5EventListener* m_eventListener;
	const ResourceSet* m_resource;		//ssbp
	const SsEffectModel* m_effectData;	//エフェクトのパラメータデータ

	//Modelに記載されているエミッタのリスト
	std::vector<SsEffectEmitter*> m_emmiterList;
	std::vector<SsEffectEmitter*> m_updateList;

	//ランダムシード
	int	m_mySeed;

	float m_nowFrame;
	float m_targetFrame;

	size_t m_effectTimeLength;

	bool m_infinite;	//無限に発生出来るかどうか

	bool m_isPlay;
	bool m_isLoop;

	int  m_seedOffset;
	bool m_isWarningData;

	//親になるスプライト
	const CustomSprite* m_parentSprite;

	int m_drawSpritecount;


private:
	void particleDraw(SsEffectEmitter* e, double t, SsEffectEmitter* parent, particleDrawData* plp, const std::vector<TextuerData>& textures);
	void initEmitter(SsEffectEmitter* e, const SsEffectNode* node);


public:
	SsEffectRenderV2(const ResourceSet* resource, SS5EventListener* eventListener, const std::string& effectName, int seed);
	~SsEffectRenderV2();

	void play(){ m_isPlay = true; }
	void stop(){ m_isPlay = false; }
	void setLoop(bool flag){ m_isLoop = flag; }
	bool isplay() const{ return m_isPlay; }
	bool isloop() const{ return m_isLoop; }

	void setFrame(float frame){
		m_nowFrame = frame;
	}
	float getFrame() const{ return m_nowFrame; }

	void update();
	void draw(const std::vector<TextuerData>& textures);

private:
	void initialize();

public:
	size_t getEffectTimeLength();

#if 0
	int	getCurrentFPS();
#endif

	void drawSprite(
		const CellRef* refCell,
		SsRenderBlendType blendType,
		const Matrix& localMatrix,
		SSColor4B	color,
		TextureID textureId
	);

	void setSeedOffset(int offset);
	bool isInfinity() const{ return m_infinite; }
	bool isWarning() const{ return m_isWarningData; }

	//親になるスプライトを設定する
	void setParentSprite(const CustomSprite* sprite) { m_parentSprite = sprite; }

	int	getDrawSpriteCount() const{ return m_drawSpritecount; }
};

} //namespace ss
