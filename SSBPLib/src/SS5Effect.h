#pragma once
#include <vector>
#include "SS5PlayerTypes.h"
#include "effect/sstypes.h"
#include "math/Matrix.h"
#include "player/PlayerSetting.h"
#include "player/EffectPartStatus.h"

namespace ss{
class SS5EventListener;
class SsEffectModel;
class SsEffectNode;
class SsEffectEmitter;
struct particleDrawData;
struct CellRef;
class ResourceSet;


class SS5Effect{
private:
	SS5EventListener* m_eventListener;
	const ResourceSet* m_resource;		//ssbp
	const SsEffectModel* m_effectData;	//エフェクトのパラメータデータ
	std::vector<TextureID> m_textures;	//テクスチャ

	//Modelに記載されているエミッタのリスト
	std::vector<SsEffectEmitter*> m_emmiterList;
	std::vector<SsEffectEmitter*> m_updateList;

	//ランダムシード
	int	m_mySeed;

	float m_nowFrame;		//フレーム(小数点を考慮)	//memo:nowというよりもtotalな気がする
	float m_targetFrame;	//こちらのフレームの値でdrawされる

	size_t m_effectTimeLength;

	bool m_infinite;	//無限に発生出来るかどうか

	bool m_isPlay;
	bool m_isLoop;

	int  m_seedOffset;
	bool m_isWarningData;

private:
	void particleDraw(SsEffectEmitter* e, double time, SsEffectEmitter* parent, const particleDrawData* plp);
	void initEmitter(SsEffectEmitter* e, const SsEffectNode* node);


public:
	SS5Effect(const ResourceSet* resource, SS5EventListener* eventListener, const std::string& effectName, int seed);
	~SS5Effect();

	void play(){ m_isPlay = true; }
	void stop(){ m_isPlay = false; }
	void setLoop(bool flag){ m_isLoop = flag; }
	bool isPlay() const{ return m_isPlay; }
	bool isLoop() const{ return m_isLoop; }

	void setFrame(float frame){
		m_nowFrame = frame;
	}
	float getFrame() const{ return m_nowFrame; }

	void update(float dt);
	void draw();

private:
	void initialize();

public:
	size_t getEffectTimeLength() const;
	int	getFPS() const;


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

public:
	//各種設定
	void setRootMatrix(const Matrix& matrix);
	void setPosition(float x, float y);
	void setRotation(float x, float y, float z);
	void setScale(float x, float y);
	void setAlpha(float a);						 /*[0:1]*/
	void setColor(float r, float g, float b);	 /*[0:1]*/
private:
	PlayerSetting m_playerSetting;

public:
	//todo:これは後でイベント化する
	void effectUpdate(
		const Matrix& parentMatrix, float parentAlpha,
		const EffectPartStatus& effectAttribute, int parentSeedOffset, int parentFrame
	){
		bool isValid = effectAttribute.isValidFrame(parentFrame);
		//有効フレーム&&未再生 --> 再生開始のタイミング
		if(isValid && !isPlay()){
			setFrame(effectAttribute.m_startTime);	//再生開始時間
			play();
		}

		setAlpha(parentAlpha);
		setRootMatrix(parentMatrix);
		
		if(isValid){
			if(effectAttribute.m_independent){
				//独立動作
				setLoop(true);
				update(1.0f/60.0f * effectAttribute.m_speed);	//・・・というより独立動作なのだからこの外側でupdateするべき。dtは渡さないので今は適当に値入れとく
			}
			else{
				float nextFrame = effectAttribute.getFrame(parentFrame);

				setSeedOffset(parentSeedOffset);
				setFrame(nextFrame);
				update(0);
			}
		}
	}
};

} //namespace ss
