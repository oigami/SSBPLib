#include "SsEffectElement.h"
#include "effect/ssplayer_effect2.h"
#include "player/DataArrayReader.h"

namespace ss{

SsEffectElementBase* SsEffectElementBase::create(SsEffectFunctionType type)
{
	switch(type){
	case SsEffectFunctionType::Basic:			//基本情報
		return new ParticleElementBasic();

	case SsEffectFunctionType::RndSeedChange:	//シード上書き
		return new ParticleElementRndSeedChange();

	case SsEffectFunctionType::Delay:			//発生：タイミング
		return new ParticleElementDelay();
	
	case SsEffectFunctionType::Gravity:			//重力を加える
		return new ParticleElementGravity();
	
	case SsEffectFunctionType::Position:		//座標：生成時
		return new ParticleElementPosition();
	
	case SsEffectFunctionType::Rotation:		//Z回転を追加
		return new ParticleElementRotation();
	
	case SsEffectFunctionType::TransRotation:	//Z回転速度変更
		return new ParticleElementRotationTrans();
	
	case SsEffectFunctionType::TransSpeed:		//速度：変化
		return new ParticleElementTransSpeed();
	
	case SsEffectFunctionType::TangentialAcceleration:	//接線加速度
		return new ParticleElementTangentialAcceleration();
	
	case SsEffectFunctionType::InitColor:		//カラーRGBA：生成時
		return new ParticleElementInitColor();
	
	case SsEffectFunctionType::TransColor:		//カラーRGB：変化
		return new ParticleElementTransColor();
	
	case SsEffectFunctionType::AlphaFade:		//フェード
		return new ParticleElementAlphaFade();
	
	case SsEffectFunctionType::Size:			//スケール：生成時
		return new ParticleElementSize();
	
	case SsEffectFunctionType::TransSize:		//スケール：変化
		return new ParticleElementTransSize();
	
	case SsEffectFunctionType::PointGravity:	//重力点の追加
		return new ParticlePointGravity();
	
	case SsEffectFunctionType::TurnToDirectionEnabled:	//進行方向に向ける
		return new ParticleTurnToDirectionEnabled();
	
	case SsEffectFunctionType::InfiniteEmitEnabled:
		return new ParticleInfiniteEmitEnabled();
	
	default:
		return nullptr;
	}
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementBasic::ParticleElementBasic()
	: m_priority(64)
	, m_maximumParticle(50)
	, m_attimeCreate(1)
	, m_interval(1)
	, m_lifetime(30)
	, m_speedMinValue(5.0f)
	, m_speedMaxValue(5.0f)
	, m_lifespanMinValue(30)
	, m_lifespanMaxValue(30)
	, m_angle(0.0f)
	, m_angleVariance(45.0f)
{
}

void ParticleElementBasic::readData(DataArrayReader& reader)
{
	m_priority			=reader.readU32();
	m_maximumParticle	=reader.readU32();
	m_attimeCreate		=reader.readU32();
	m_interval			=reader.readU32();
	m_lifetime			=reader.readU32();
	m_speedMinValue		=reader.readFloat();
	m_speedMaxValue		=reader.readFloat();
	m_lifespanMinValue	=reader.readU32();
	m_lifespanMaxValue	=reader.readU32();
	m_angle				=reader.readFloat();
	m_angleVariance		=reader.readFloat();
}

void ParticleElementBasic::initalizeEffect(SsEffectEmitter* e) const
{
	e->priority = m_priority;

	//エミッターパラメータ
	e->emitter.emitmax = m_maximumParticle;
	e->emitter.interval = m_interval;
	e->emitter.life = m_lifetime;
	e->emitter.emitnum = m_attimeCreate;
	e->emitter.particleLife = 10;//
	e->emitter.Infinite = false;
	e->emitter.loopGen = 0;


	//パーティクルパラメータ
	e->emitter.particleLife = m_lifespanMinValue;
	e->emitter.particleLife2 = m_lifespanMaxValue - m_lifespanMinValue;

	e->particle.scale = Vector2(1.0f, 1.0f);
	e->particle.startcolor = SsU8Color(255, 255, 255, 255);
	e->particle.endcolor = SsU8Color(255, 255, 255, 255);

	e->particle.speed = m_speedMinValue;
	e->particle.speed2 = m_speedMaxValue - m_speedMinValue;

	e->particle.angle = SSDegToRad((m_angle + 90.0f));
	e->particle.angleVariance = SSDegToRad(m_angleVariance);

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
ParticleElementRndSeedChange::ParticleElementRndSeedChange()
	: m_Seed(0)
{
}

void ParticleElementRndSeedChange::readData(DataArrayReader& reader)
{
	m_Seed = reader.readU32();
}

void ParticleElementRndSeedChange::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.userOverrideRSeed = true;

	e->particle.overrideRSeed = m_Seed + SEED_MAGIC;
	e->emitterSeed = m_Seed + SEED_MAGIC;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementDelay::ParticleElementDelay()
	: m_DelayTime(0)
{
}

void ParticleElementDelay::readData(DataArrayReader& reader)
{
	m_DelayTime = reader.readU32();
}

void ParticleElementDelay::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.delay = m_DelayTime;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementGravity::ParticleElementGravity()
	: m_Gravity(0, -3.0f)
{
}

void ParticleElementGravity::readData(DataArrayReader& reader)
{
	m_Gravity.x = reader.readFloat();
	m_Gravity.y = reader.readFloat();
}

void ParticleElementGravity::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useGravity = true;
	e->particle.gravity = m_Gravity;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementPosition::ParticleElementPosition()
	: m_OffsetXMinValue(0)
	, m_OffsetXMaxValue(0)
	, m_OffsetYMinValue(0)
	, m_OffsetYMaxValue(0)
{
}

void ParticleElementPosition::readData(DataArrayReader& reader)
{
	m_OffsetXMinValue = reader.readFloat();
	m_OffsetXMaxValue = reader.readFloat();
	m_OffsetYMinValue = reader.readFloat();
	m_OffsetYMaxValue = reader.readFloat();
}

void ParticleElementPosition::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useOffset = true;
	e->particle.offset = Vector2(m_OffsetXMinValue, m_OffsetYMinValue);
	e->particle.offset2 = Vector2(m_OffsetXMaxValue - m_OffsetXMinValue, m_OffsetYMaxValue - m_OffsetYMinValue);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementRotation::ParticleElementRotation()
	: m_RotationMinValue(0)
	, m_RotationMaxValue(0)
	, m_RotationAddMinValue(0)
	, m_RotationAddMaxValue(0)
{
}

void ParticleElementRotation::readData(DataArrayReader& reader)
{
	m_RotationMinValue = reader.readFloat();
	m_RotationMaxValue = reader.readFloat();
	m_RotationAddMinValue = reader.readFloat();
	m_RotationAddMaxValue = reader.readFloat();
}

void ParticleElementRotation::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useRotation = true;
	e->particle.rotation = m_RotationMinValue;
	e->particle.rotation2 = m_RotationMaxValue - m_RotationMinValue;

	e->particle.rotationAdd = m_RotationAddMinValue;
	e->particle.rotationAdd2 = m_RotationAddMaxValue - m_RotationAddMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementRotationTrans::ParticleElementRotationTrans()
	: m_RotationFactor(0)
	, m_EndLifeTimePer(75)
{
}

void ParticleElementRotationTrans::readData(DataArrayReader& reader)
{
	m_RotationFactor = reader.readFloat();
	m_EndLifeTimePer = reader.readFloat();
}

void ParticleElementRotationTrans::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useRotationTrans = true;
	e->particle.rotationFactor = m_RotationFactor;
	e->particle.endLifeTimePer = m_EndLifeTimePer / 100.0f;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTransSpeed::ParticleElementTransSpeed()
	: m_SpeedMinValue(0)
	, m_SpeedMaxValue(0)
{
}

void ParticleElementTransSpeed::readData(DataArrayReader& reader)
{
	m_SpeedMinValue = reader.readFloat();
	m_SpeedMaxValue = reader.readFloat();
}

void ParticleElementTransSpeed::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useTransSpeed = true;
	e->particle.transSpeed = m_SpeedMinValue;
	e->particle.transSpeed2 = m_SpeedMaxValue - m_SpeedMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTangentialAcceleration::ParticleElementTangentialAcceleration()
	: m_AccelerationMinValue(0)
	, m_AccelerationMaxValue(0)
{
}

void ParticleElementTangentialAcceleration::readData(DataArrayReader& reader)
{
	m_AccelerationMinValue = reader.readFloat();
	m_AccelerationMaxValue = reader.readFloat();
}

void ParticleElementTangentialAcceleration::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useTanAccel = true;
	e->particle.tangentialAccel = m_AccelerationMinValue;
	e->particle.tangentialAccel2 = (m_AccelerationMaxValue - m_AccelerationMinValue);
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
ParticleElementInitColor::ParticleElementInitColor()
	: m_ColorMinValue(255, 255, 255, 255)
	, m_ColorMaxValue(255, 255, 255, 255)
{
}

void ParticleElementInitColor::readData(DataArrayReader& reader)
{
	unsigned int colorMinValue = reader.readU32();			//設定カラー最小
	unsigned int colorMaxValue = reader.readU32();			//設定カラー最大

	int a = (colorMinValue & 0xFF000000) >> 24;
	int r = (colorMinValue & 0x00FF0000) >> 16;
	int g = (colorMinValue & 0x0000FF00) >> 8;
	int b = (colorMinValue & 0x000000FF) >> 0;
	SsU8Color mincol(r, g, b, a);
	a = (colorMaxValue & 0xFF000000) >> 24;
	r = (colorMaxValue & 0x00FF0000) >> 16;
	g = (colorMaxValue & 0x0000FF00) >> 8;
	b = (colorMaxValue & 0x000000FF) >> 0;
	SsU8Color maxcol(r, g, b, a);
	m_ColorMinValue = mincol;			//設定カラー最小
	m_ColorMaxValue = maxcol;			//設定カラー最大
}

void ParticleElementInitColor::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useColor = true;

	SsU8Color color1 = m_ColorMinValue;
	SsU8Color color2 = m_ColorMaxValue;

	getRange(color1.a, color2.a, e->particle.initColor.a, e->particle.initColor2.a);
	getRange(color1.r, color2.r, e->particle.initColor.r, e->particle.initColor2.r);
	getRange(color1.g, color2.g, e->particle.initColor.g, e->particle.initColor2.g);
	getRange(color1.b, color2.b, e->particle.initColor.b, e->particle.initColor2.b);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTransColor::ParticleElementTransColor()
	: m_ColorMinValue(255, 255, 255, 255)
	, m_ColorMaxValue(255, 255, 255, 255)
{
}

void ParticleElementTransColor::readData(DataArrayReader& reader)
{
	unsigned int colorMinValue = reader.readU32();			//設定カラー最小
	unsigned int colorMaxValue = reader.readU32();			//設定カラー最大

	int a = (colorMinValue & 0xFF000000) >> 24;
	int r = (colorMinValue & 0x00FF0000) >> 16;
	int g = (colorMinValue & 0x0000FF00) >> 8;
	int b = (colorMinValue & 0x000000FF) >> 0;
	SsU8Color mincol(r, g, b, a);
	a = (colorMaxValue & 0xFF000000) >> 24;
	r = (colorMaxValue & 0x00FF0000) >> 16;
	g = (colorMaxValue & 0x0000FF00) >> 8;
	b = (colorMaxValue & 0x000000FF) >> 0;
	SsU8Color maxcol(r, g, b, a);
	m_ColorMinValue = mincol;			//設定カラー最小
	m_ColorMaxValue = maxcol;			//設定カラー最大
}

void ParticleElementTransColor::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useTransColor = true;

	SsU8Color color1 = m_ColorMinValue;
	SsU8Color color2 = m_ColorMaxValue;

	getRange(color1.a, color2.a, e->particle.transColor.a, e->particle.transColor2.a);
	getRange(color1.r, color2.r, e->particle.transColor.r, e->particle.transColor2.r);
	getRange(color1.g, color2.g, e->particle.transColor.g, e->particle.transColor2.g);
	getRange(color1.b, color2.b, e->particle.transColor.b, e->particle.transColor2.b);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementAlphaFade::ParticleElementAlphaFade()
	: m_disprangeMinValue(25)
	, m_disprangeMaxValue(75)
{
}

void ParticleElementAlphaFade::readData(DataArrayReader& reader)
{
	m_disprangeMinValue = reader.readFloat();
	m_disprangeMaxValue = reader.readFloat();
}

void ParticleElementAlphaFade::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useAlphaFade = true;
	e->particle.alphaFade = m_disprangeMinValue;
	e->particle.alphaFade2 = m_disprangeMaxValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementSize::ParticleElementSize()
	: m_SizeXMinValue(1.0f)
	, m_SizeXMaxValue(1.0f)
	, m_SizeYMinValue(1.0f)
	, m_SizeYMaxValue(1.0f)
	, m_ScaleFactorMinValue(1.0f)
	, m_ScaleFactorMaxValue(1.0f)
{
}

void ParticleElementSize::readData(DataArrayReader& reader)
{
	m_SizeXMinValue = reader.readFloat();
	m_SizeXMaxValue = reader.readFloat();
	m_SizeYMinValue = reader.readFloat();
	m_SizeYMaxValue = reader.readFloat();
	m_ScaleFactorMinValue = reader.readFloat();
	m_ScaleFactorMaxValue = reader.readFloat();
}

void ParticleElementSize::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useInitScale = true;

	e->particle.scale.x = m_SizeXMinValue;
	e->particle.scaleRange.x = m_SizeXMaxValue - m_SizeXMinValue;

	e->particle.scale.y = m_SizeYMinValue;
	e->particle.scaleRange.y = m_SizeYMaxValue - m_SizeYMinValue;

	e->particle.scaleFactor = m_ScaleFactorMinValue;
	e->particle.scaleFactor2 = m_ScaleFactorMaxValue - m_ScaleFactorMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTransSize::ParticleElementTransSize()
	: m_SizeXMinValue(1.0f)
	, m_SizeXMaxValue(1.0f)
	, m_SizeYMinValue(1.0f)
	, m_SizeYMaxValue(1.0f)
	, m_ScaleFactorMinValue(1.0f)
	, m_ScaleFactorMaxValue(1.0f)
{
}

void ParticleElementTransSize::readData(DataArrayReader& reader)
{
	m_SizeXMinValue = reader.readFloat();
	m_SizeXMaxValue = reader.readFloat();
	m_SizeYMinValue = reader.readFloat();
	m_SizeYMaxValue = reader.readFloat();
	m_ScaleFactorMinValue = reader.readFloat();
	m_ScaleFactorMaxValue = reader.readFloat();
}

void ParticleElementTransSize::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useTransScale = true;

	e->particle.transscale.x = m_SizeXMinValue;
	e->particle.transscaleRange.x = m_SizeXMaxValue - m_SizeXMinValue;

	e->particle.transscale.y = m_SizeYMinValue;
	e->particle.transscaleRange.y = m_SizeYMaxValue - m_SizeYMinValue;

	e->particle.transscaleFactor = m_ScaleFactorMinValue;
	e->particle.transscaleFactor2 = m_ScaleFactorMaxValue - m_ScaleFactorMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticlePointGravity::ParticlePointGravity()
	: m_Position(0, 0), m_Power(0.0f)
{
}

void ParticlePointGravity::readData(DataArrayReader& reader)
{
	m_Position.x = reader.readFloat();
	m_Position.y = reader.readFloat();
	m_Power = reader.readFloat();
}

void ParticlePointGravity::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.usePGravity = true;
	e->particle.gravityPos = m_Position;
//	e->particle.gravityPower = m_Power / 100.0f;
	e->particle.gravityPower = m_Power;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleTurnToDirectionEnabled::ParticleTurnToDirectionEnabled()
	: m_Rotation(0.0f)
{
}

void ParticleTurnToDirectionEnabled::readData(DataArrayReader& reader)
{
	m_Rotation = reader.readFloat();
}

void ParticleTurnToDirectionEnabled::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useTurnDirec = true;
	e->particle.direcRotAdd = m_Rotation;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleInfiniteEmitEnabled::ParticleInfiniteEmitEnabled()
{
}

void ParticleInfiniteEmitEnabled::readData(DataArrayReader& reader)
{
	int dummy = reader.readS32();	//ダミーデータが入っているので読み込む必要がある
}

void ParticleInfiniteEmitEnabled::initalizeEffect(SsEffectEmitter* e) const
{
	e->emitter.Infinite = true;
}



} //namspace ss
