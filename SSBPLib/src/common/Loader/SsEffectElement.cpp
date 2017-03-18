#include "SsEffectElement.h"
#include "common/Animator/ssplayer_effect2.h"

namespace ss
{


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementBasic::initalizeEffect(SsEffectEmitter* e)
{
	e->priority = this->priority;

	//エミッターパラメータ
	e->emitter.emitmax = this->maximumParticle;
	e->emitter.interval = this->interval;
	e->emitter.life = this->lifetime;
	e->emitter.emitnum = this->attimeCreate;
	e->emitter.particleLife = 10;//
	e->emitter.Infinite = false;
	e->emitter.loopGen = 0;


	//パーティクルパラメータ
	e->emitter.particleLife = this->lifespanMinValue;
	e->emitter.particleLife2 = this->lifespanMaxValue - this->lifespanMinValue;

	e->particle.scale = Vector2(1.0f, 1.0f);
	e->particle.startcolor = SsU8Color(255, 255, 255, 255);
	e->particle.endcolor = SsU8Color(255, 255, 255, 255);

	e->particle.speed = this->speedMinValue;
	e->particle.speed2 = this->speedMaxValue - this->speedMinValue;

	e->particle.angle = SSDegToRad((this->angle + 90.0f));
	e->particle.angleVariance = SSDegToRad(this->angleVariance);

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


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementRndSeedChange::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.userOverrideRSeed = true;

	e->particle.overrideRSeed = this->Seed + SEED_MAGIC;
	e->emitterSeed = this->Seed + SEED_MAGIC;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementDelay::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.delay = this->DelayTime;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementGravity::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useGravity = true;
	e->particle.gravity = this->Gravity;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementPosition::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useOffset = true;
	e->particle.offset = Vector2(this->OffsetXMinValue, this->OffsetYMinValue);
	e->particle.offset2 = Vector2(this->OffsetXMaxValue - this->OffsetXMinValue, this->OffsetYMaxValue - this->OffsetYMinValue);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementRotation::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useRotation = true;
	e->particle.rotation = this->RotationMinValue;
	e->particle.rotation2 = this->RotationMaxValue - this->RotationMinValue;

	e->particle.rotationAdd = this->RotationAddMinValue;
	e->particle.rotationAdd2 = this->RotationAddMaxValue - this->RotationAddMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementRotationTrans::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useRotationTrans = true;
	e->particle.rotationFactor = this->RotationFactor;
	e->particle.endLifeTimePer = this->EndLifeTimePer / 100.0f;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementTransSpeed::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useTransSpeed = true;
	e->particle.transSpeed = this->SpeedMinValue;
	e->particle.transSpeed2 = this->SpeedMaxValue - this->SpeedMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementTangentialAcceleration::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useTanAccel = true;
	e->particle.tangentialAccel = this->AccelerationMinValue;
	e->particle.tangentialAccel2 = (this->AccelerationMaxValue - this->AccelerationMinValue);
}

static void getRange(u8 a, u8 b, u8& min, u8& diff)
{
	min = a < b ? a : b;
	u8 max = a < b ? b : a;

	diff = (max - min);
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementInitColor::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useColor = true;

	SsU8Color color1 = this->ColorMinValue;
	SsU8Color color2 = this->ColorMaxValue;

	getRange(color1.a, color2.a, e->particle.initColor.a, e->particle.initColor2.a);
	getRange(color1.r, color2.r, e->particle.initColor.r, e->particle.initColor2.r);
	getRange(color1.g, color2.g, e->particle.initColor.g, e->particle.initColor2.g);
	getRange(color1.b, color2.b, e->particle.initColor.b, e->particle.initColor2.b);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementTransColor::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useTransColor = true;

	SsU8Color color1 = this->ColorMinValue;
	SsU8Color color2 = this->ColorMaxValue;

	getRange(color1.a, color2.a, e->particle.transColor.a, e->particle.transColor2.a);
	getRange(color1.r, color2.r, e->particle.transColor.r, e->particle.transColor2.r);
	getRange(color1.g, color2.g, e->particle.transColor.g, e->particle.transColor2.g);
	getRange(color1.b, color2.b, e->particle.transColor.b, e->particle.transColor2.b);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementAlphaFade::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useAlphaFade = true;
	e->particle.alphaFade = this->disprangeMinValue;
	e->particle.alphaFade2 = this->disprangeMaxValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementSize::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useInitScale = true;

	e->particle.scale.x = this->SizeXMinValue;
	e->particle.scaleRange.x = this->SizeXMaxValue - this->SizeXMinValue;

	e->particle.scale.y = this->SizeYMinValue;
	e->particle.scaleRange.y = this->SizeYMaxValue - this->SizeYMinValue;

	e->particle.scaleFactor = this->ScaleFactorMinValue;
	e->particle.scaleFactor2 = this->ScaleFactorMaxValue - this->ScaleFactorMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleElementTransSize::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useTransScale = true;

	e->particle.transscale.x = this->SizeXMinValue;
	e->particle.transscaleRange.x = this->SizeXMaxValue - this->SizeXMinValue;

	e->particle.transscale.y = this->SizeYMinValue;
	e->particle.transscaleRange.y = this->SizeYMaxValue - this->SizeYMinValue;

	e->particle.transscaleFactor = this->ScaleFactorMinValue;
	e->particle.transscaleFactor2 = this->ScaleFactorMaxValue - this->ScaleFactorMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticlePointGravity::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.usePGravity = true;
	e->particle.gravityPos = this->Position;
//	e->particle.gravityPower = this->Power / 100.0f;
	e->particle.gravityPower = this->Power;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleTurnToDirectionEnabled::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useTurnDirec = true;
	e->particle.direcRotAdd = this->Rotation;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void ParticleInfiniteEmitEnabled::initalizeEffect(SsEffectEmitter* e)
{
	e->emitter.Infinite = true;
}





#if 0
void	SsEffectFunctionExecuter::initializeEffect(SsEffectBehavior* beh, SsEffectEmitter* emmiter)
{
	for(SsEffectElementBase* e : beh->plist)
	{
		e->initalizeEffect(emmiter);
	}
}
#endif


} //namspace ss

















