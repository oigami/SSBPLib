#include <stdio.h>
#include <cstdlib>

#include "ssplayer_effect2.h"
#include "ssplayer_effectfunction.h"

namespace ss
{


class EffectFuncBase
{
public:
	EffectFuncBase(){}
	virtual ~EffectFuncBase(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* emmiter) {}
};



//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementBasic : public EffectFuncBase
{
public:
	FuncParticleElementBasic(){}
	virtual ~FuncParticleElementBasic(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementBasic* source = static_cast<ParticleElementBasic*>(ele);
		e->priority = source->priority;

		//エミッターパラメータ
		e->emitter.emitmax = source->maximumParticle;
		e->emitter.interval = source->interval;
		e->emitter.life = source->lifetime;
		e->emitter.emitnum = source->attimeCreate;
		e->emitter.particleLife = 10;//
		e->emitter.Infinite = false;
		e->emitter.loopGen = 0;


		//パーティクルパラメータ
		e->emitter.particleLife = source->lifespanMinValue;
		e->emitter.particleLife2 = source->lifespanMaxValue - source->lifespanMinValue;

		e->particle.scale = Vector2(1.0f, 1.0f);
		e->particle.startcolor = SsU8Color(255, 255, 255, 255);
		e->particle.endcolor = SsU8Color(255, 255, 255, 255);

		e->particle.speed = source->speedMinValue;
		e->particle.speed2 = source->speedMaxValue - source->speedMinValue;

		e->particle.angle = SSDegToRad((source->angle + 90.0f));
		e->particle.angleVariance = SSDegToRad(source->angleVariance);

		e->particle.useTanAccel = false;

		//重力
		e->particle.useGravity = false;
		e->particle.gravity = Vector2(0, 0);

		//オフセット
		e->particle.useOffset = false;
		e->particle.offset = Vector2(0, 0);
		e->particle.offset2 = Vector2(0, 0);


		//回転
		e->particle.useRotation = false;
		e->particle.useRotationTrans = false;

		//カラー
		e->particle.useColor = false;
		e->particle.useTransColor = false;

		//スケール
		e->particle.useInitScale = false;
		e->particle.useTransScale = false;

		e->particle.delay = 0;

		e->particle.usePGravity = false;

		e->particle.useTransColor = false;
		e->particle.useInitScale = false;
		e->particle.usePGravity = false;
		e->particle.useAlphaFade = false;
		e->particle.useTransSpeed = false;
		e->particle.useTurnDirec = false;
		e->particle.userOverrideRSeed = false;
	}


};
static FuncParticleElementBasic		funcBasic;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementRndSeedChange : public EffectFuncBase
{
public:
	FuncParticleElementRndSeedChange(){}
	virtual ~FuncParticleElementRndSeedChange(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementRndSeedChange* source = static_cast<ParticleElementRndSeedChange*>(ele);
		e->particle.userOverrideRSeed = true;

		e->particle.overrideRSeed = source->Seed + SEED_MAGIC;
		e->emitterSeed = source->Seed + SEED_MAGIC;
	}
};
static FuncParticleElementRndSeedChange		funcRndSeedChange;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementDelay : public EffectFuncBase
{
public:
	FuncParticleElementDelay(){}
	virtual ~FuncParticleElementDelay(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementDelay* source = static_cast<ParticleElementDelay*>(ele);
		e->particle.delay = source->DelayTime;

	}
};
static FuncParticleElementDelay		funcDelay;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementGravity : public EffectFuncBase
{
public:
	FuncParticleElementGravity(){}
	virtual ~FuncParticleElementGravity(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementGravity* source = static_cast<ParticleElementGravity*>(ele);
		e->particle.useGravity = true;
		e->particle.gravity = source->Gravity;

	}
};
static FuncParticleElementGravity		funcGravity;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementPosition : public EffectFuncBase
{
public:
	FuncParticleElementPosition(){}
	virtual ~FuncParticleElementPosition(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementPosition* source = static_cast<ParticleElementPosition*>(ele);
		e->particle.useOffset = true;
		e->particle.offset = Vector2(source->OffsetXMinValue, source->OffsetYMinValue);
		e->particle.offset2 = Vector2(source->OffsetXMaxValue - source->OffsetXMinValue, source->OffsetYMaxValue - source->OffsetYMinValue);
	}
};
static FuncParticleElementPosition		funcPosition;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementRotation : public EffectFuncBase
{
public:
	FuncParticleElementRotation(){}
	virtual ~FuncParticleElementRotation(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementRotation* source = static_cast<ParticleElementRotation*>(ele);
		e->particle.useRotation = true;
		e->particle.rotation = source->RotationMinValue;
		e->particle.rotation2 = source->RotationMaxValue - source->RotationMinValue;

		e->particle.rotationAdd = source->RotationAddMinValue;
		e->particle.rotationAdd2 = source->RotationAddMaxValue - source->RotationAddMinValue;

	}
};
static FuncParticleElementRotation		funcRotation;



//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementRotationTrans : public EffectFuncBase
{
public:
	FuncParticleElementRotationTrans(){}
	virtual ~FuncParticleElementRotationTrans(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementRotationTrans* source = static_cast<ParticleElementRotationTrans*>(ele);
		e->particle.useRotationTrans = true;
		e->particle.rotationFactor = source->RotationFactor;
		e->particle.endLifeTimePer = source->EndLifeTimePer / 100.0f;
	}
};
static FuncParticleElementRotationTrans		funcRotationTrans;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTransSpeed : public EffectFuncBase
{
public:
	FuncParticleElementTransSpeed(){}
	virtual ~FuncParticleElementTransSpeed(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementTransSpeed* source = static_cast<ParticleElementTransSpeed*>(ele);
		e->particle.useTransSpeed = true;
		e->particle.transSpeed = source->SpeedMinValue;
		e->particle.transSpeed2 = source->SpeedMaxValue - source->SpeedMinValue;
	}
};
static FuncParticleElementTransSpeed		funcTransSpeed;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTangentialAcceleration : public EffectFuncBase
{
public:
	FuncParticleElementTangentialAcceleration(){}
	virtual ~FuncParticleElementTangentialAcceleration(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementTangentialAcceleration* source = static_cast<ParticleElementTangentialAcceleration*>(ele);
		e->particle.useTanAccel = true;
		e->particle.tangentialAccel = source->AccelerationMinValue;
		e->particle.tangentialAccel2 = (source->AccelerationMaxValue - source->AccelerationMinValue);

	}
};
static FuncParticleElementTangentialAcceleration		funcTangentialAcceleration;

static void getRange(u8 a, u8 b, u8& min, u8& diff)
{
	min = a < b ? a : b;
	u8 max = a < b ? b : a;

	diff = (max - min);
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementInitColor : public EffectFuncBase
{
public:
	FuncParticleElementInitColor(){}
	virtual ~FuncParticleElementInitColor(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementInitColor* source = static_cast<ParticleElementInitColor*>(ele);
		e->particle.useColor = true;

		SsU8Color color1 = source->ColorMinValue;
		SsU8Color color2 = source->ColorMaxValue;

		getRange(color1.a, color2.a, e->particle.initColor.a, e->particle.initColor2.a);
		getRange(color1.r, color2.r, e->particle.initColor.r, e->particle.initColor2.r);
		getRange(color1.g, color2.g, e->particle.initColor.g, e->particle.initColor2.g);
		getRange(color1.b, color2.b, e->particle.initColor.b, e->particle.initColor2.b);

	}
};
static FuncParticleElementInitColor		funcInitColor;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTransColor : public EffectFuncBase
{
public:
	FuncParticleElementTransColor(){}
	virtual ~FuncParticleElementTransColor(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementTransColor* source = static_cast<ParticleElementTransColor*>(ele);

		e->particle.useTransColor = true;

		SsU8Color color1 = source->ColorMinValue;
		SsU8Color color2 = source->ColorMaxValue;

		getRange(color1.a, color2.a, e->particle.transColor.a, e->particle.transColor2.a);
		getRange(color1.r, color2.r, e->particle.transColor.r, e->particle.transColor2.r);
		getRange(color1.g, color2.g, e->particle.transColor.g, e->particle.transColor2.g);
		getRange(color1.b, color2.b, e->particle.transColor.b, e->particle.transColor2.b);
	}
};
static FuncParticleElementTransColor		funcTransColor;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementAlphaFade : public EffectFuncBase
{
public:
	FuncParticleElementAlphaFade(){}
	virtual ~FuncParticleElementAlphaFade(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementAlphaFade* source = static_cast<ParticleElementAlphaFade*>(ele);
		e->particle.useAlphaFade = true;
		e->particle.alphaFade = source->disprangeMinValue;
		e->particle.alphaFade2 = source->disprangeMaxValue;

	}
};
static FuncParticleElementAlphaFade		funcAlphaFade;



//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementSize : public EffectFuncBase
{
public:
	FuncParticleElementSize(){}
	virtual ~FuncParticleElementSize(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementSize* source = static_cast<ParticleElementSize*>(ele);

		e->particle.useInitScale = true;

		e->particle.scale.x = source->SizeXMinValue;
		e->particle.scaleRange.x = source->SizeXMaxValue - source->SizeXMinValue;

		e->particle.scale.y = source->SizeYMinValue;
		e->particle.scaleRange.y = source->SizeYMaxValue - source->SizeYMinValue;

		e->particle.scaleFactor = source->ScaleFactorMinValue;
		e->particle.scaleFactor2 = source->ScaleFactorMaxValue - source->ScaleFactorMinValue;
	}
};
static FuncParticleElementSize		funcSize;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTransSize : public EffectFuncBase
{
public:
	FuncParticleElementTransSize(){}
	virtual ~FuncParticleElementTransSize(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleElementTransSize* source = static_cast<ParticleElementTransSize*>(ele);
		e->particle.useTransScale = true;

		e->particle.transscale.x = source->SizeXMinValue;
		e->particle.transscaleRange.x = source->SizeXMaxValue - source->SizeXMinValue;

		e->particle.transscale.y = source->SizeYMinValue;
		e->particle.transscaleRange.y = source->SizeYMaxValue - source->SizeYMinValue;

		e->particle.transscaleFactor = source->ScaleFactorMinValue;
		e->particle.transscaleFactor2 = source->ScaleFactorMaxValue - source->ScaleFactorMinValue;

	}
};
static FuncParticleElementTransSize		funcTransSize;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticlePointGravity : public EffectFuncBase
{
public:
	FuncParticlePointGravity(){}
	virtual ~FuncParticlePointGravity(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticlePointGravity* source = static_cast<ParticlePointGravity*>(ele);
		e->particle.usePGravity = true;
		e->particle.gravityPos = source->Position;
//		e->particle.gravityPower = source->Power / 100.0f;
		e->particle.gravityPower = source->Power;

	}
};
static FuncParticlePointGravity		funcPointGravity;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleTurnToDirectionEnabled : public EffectFuncBase
{
public:
	FuncParticleTurnToDirectionEnabled(){}
	virtual ~FuncParticleTurnToDirectionEnabled(){}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		ParticleTurnToDirectionEnabled* source = static_cast<ParticleTurnToDirectionEnabled*>(ele);
		e->particle.useTurnDirec = true;
		e->particle.direcRotAdd = source->Rotation;
	}
};
static FuncParticleTurnToDirectionEnabled		funcTurnToDirectionEnabled;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleInfiniteEmitEnabled : public EffectFuncBase
{
public:
	FuncParticleInfiniteEmitEnabled() {}
	virtual ~FuncParticleInfiniteEmitEnabled() {}

	virtual void	initalizeEffect(SsEffectElementBase* ele, SsEffectEmitter* e)
	{
		e->emitter.Infinite = true;
	}
};
static FuncParticleInfiniteEmitEnabled		funcParticleInfiniteEmitEnabled;

//-------------------------------------------------------------------
//挙動反映クラスの呼び出しテーブル
//SsEffectFunctionTypeの順に並べること
//-------------------------------------------------------------------
static EffectFuncBase* callTable[] =
{
	0,
	&funcBasic,
	&funcRndSeedChange,
	&funcDelay,
	&funcGravity,
	&funcPosition,
	//&funcTransPosition,
	&funcRotation,
	&funcRotationTrans,
	&funcTransSpeed,
	&funcTangentialAcceleration,
	&funcInitColor,
	&funcTransColor,
	&funcAlphaFade,
	&funcSize,
	&funcTransSize,
	&funcPointGravity,
	&funcTurnToDirectionEnabled,
	&funcParticleInfiniteEmitEnabled,
};


void	SsEffectFunctionExecuter::initializeEffect(SsEffectBehavior* beh, SsEffectEmitter* emmiter)
{
	for(SsEffectElementBase* e : beh->plist)
	{
		EffectFuncBase* cf = callTable[e->getFunctionType()];
		cf->initalizeEffect(e, emmiter);
	}
}

};
















