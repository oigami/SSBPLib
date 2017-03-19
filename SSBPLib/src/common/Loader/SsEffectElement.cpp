#include "SsEffectElement.h"
#include "common/Animator/ssplayer_effect2.h"
#include "player/DataArrayReader.h"

namespace ss{

SsEffectElementBase* SsEffectElementBase::create(SsEffectFunctionType::enum_ type)
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
	: priority(64)
	, maximumParticle(50)
	, attimeCreate(1)
	, interval(1)
	, lifetime(30)
	, speedMinValue(5.0f)
	, speedMaxValue(5.0f)
	, lifespanMinValue(30)
	, lifespanMaxValue(30)
	, angle(0.0f)
	, angleVariance(45.0f)
{
}

void ParticleElementBasic::readData(DataArrayReader& reader)
{
	this->priority			=reader.readU32();
	this->maximumParticle	=reader.readU32();
	this->attimeCreate		=reader.readU32();
	this->interval			=reader.readU32();
	this->lifetime			=reader.readU32();
	this->speedMinValue		=reader.readFloat();
	this->speedMaxValue		=reader.readFloat();
	this->lifespanMinValue	=reader.readU32();
	this->lifespanMaxValue	=reader.readU32();
	this->angle				=reader.readFloat();
	this->angleVariance		=reader.readFloat();
}

void ParticleElementBasic::initalizeEffect(SsEffectEmitter* e) const
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
ParticleElementRndSeedChange::ParticleElementRndSeedChange()
	: Seed(0)
{
}

void ParticleElementRndSeedChange::readData(DataArrayReader& reader)
{
	this->Seed = reader.readU32();
}

void ParticleElementRndSeedChange::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.userOverrideRSeed = true;

	e->particle.overrideRSeed = this->Seed + SEED_MAGIC;
	e->emitterSeed = this->Seed + SEED_MAGIC;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementDelay::ParticleElementDelay()
	: DelayTime(0)
{
}

void ParticleElementDelay::readData(DataArrayReader& reader)
{
	this->DelayTime = reader.readU32();
}

void ParticleElementDelay::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.delay = this->DelayTime;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementGravity::ParticleElementGravity()
	: Gravity(0, -3.0f)
{
}

void ParticleElementGravity::readData(DataArrayReader& reader)
{
	this->Gravity.x = reader.readFloat();
	this->Gravity.y = reader.readFloat();
}

void ParticleElementGravity::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useGravity = true;
	e->particle.gravity = this->Gravity;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementPosition::ParticleElementPosition()
	: OffsetXMinValue(0)
	, OffsetXMaxValue(0)
	, OffsetYMinValue(0)
	, OffsetYMaxValue(0)
{
}

void ParticleElementPosition::readData(DataArrayReader& reader)
{
	this->OffsetXMinValue = reader.readFloat();
	this->OffsetXMaxValue = reader.readFloat();
	this->OffsetYMinValue = reader.readFloat();
	this->OffsetYMaxValue = reader.readFloat();
}

void ParticleElementPosition::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useOffset = true;
	e->particle.offset = Vector2(this->OffsetXMinValue, this->OffsetYMinValue);
	e->particle.offset2 = Vector2(this->OffsetXMaxValue - this->OffsetXMinValue, this->OffsetYMaxValue - this->OffsetYMinValue);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementRotation::ParticleElementRotation()
	: RotationMinValue(0)
	, RotationMaxValue(0)
	, RotationAddMinValue(0)
	, RotationAddMaxValue(0)
{
}

void ParticleElementRotation::readData(DataArrayReader& reader)
{
	this->RotationMinValue = reader.readFloat();
	this->RotationMaxValue = reader.readFloat();
	this->RotationAddMinValue = reader.readFloat();
	this->RotationAddMaxValue = reader.readFloat();
}

void ParticleElementRotation::initalizeEffect(SsEffectEmitter* e) const
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
ParticleElementRotationTrans::ParticleElementRotationTrans()
	: RotationFactor(0)
	, EndLifeTimePer(75)
{
}

void ParticleElementRotationTrans::readData(DataArrayReader& reader)
{
	this->RotationFactor = reader.readFloat();
	this->EndLifeTimePer = reader.readFloat();
}

void ParticleElementRotationTrans::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useRotationTrans = true;
	e->particle.rotationFactor = this->RotationFactor;
	e->particle.endLifeTimePer = this->EndLifeTimePer / 100.0f;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTransSpeed::ParticleElementTransSpeed()
	: SpeedMinValue(0)
	, SpeedMaxValue(0)
{
}

void ParticleElementTransSpeed::readData(DataArrayReader& reader)
{
	this->SpeedMinValue = reader.readFloat();
	this->SpeedMaxValue = reader.readFloat();
}

void ParticleElementTransSpeed::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useTransSpeed = true;
	e->particle.transSpeed = this->SpeedMinValue;
	e->particle.transSpeed2 = this->SpeedMaxValue - this->SpeedMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTangentialAcceleration::ParticleElementTangentialAcceleration()
	: AccelerationMinValue(0)
	, AccelerationMaxValue(0)
{
}

void ParticleElementTangentialAcceleration::readData(DataArrayReader& reader)
{
	this->AccelerationMinValue = reader.readFloat();
	this->AccelerationMaxValue = reader.readFloat();
}

void ParticleElementTangentialAcceleration::initalizeEffect(SsEffectEmitter* e) const
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
ParticleElementInitColor::ParticleElementInitColor()
	: ColorMinValue(255, 255, 255, 255)
	, ColorMaxValue(255, 255, 255, 255)
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
	this->ColorMinValue = mincol;			//設定カラー最小
	this->ColorMaxValue = maxcol;			//設定カラー最大
}

void ParticleElementInitColor::initalizeEffect(SsEffectEmitter* e) const
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
ParticleElementTransColor::ParticleElementTransColor()
	: ColorMinValue(255, 255, 255, 255)
	, ColorMaxValue(255, 255, 255, 255)
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
	this->ColorMinValue = mincol;			//設定カラー最小
	this->ColorMaxValue = maxcol;			//設定カラー最大
}

void ParticleElementTransColor::initalizeEffect(SsEffectEmitter* e) const
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
ParticleElementAlphaFade::ParticleElementAlphaFade()
	: disprangeMinValue(25)
	, disprangeMaxValue(75)
{
}

void ParticleElementAlphaFade::readData(DataArrayReader& reader)
{
	this->disprangeMinValue = reader.readFloat();
	this->disprangeMaxValue = reader.readFloat();
}

void ParticleElementAlphaFade::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useAlphaFade = true;
	e->particle.alphaFade = this->disprangeMinValue;
	e->particle.alphaFade2 = this->disprangeMaxValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementSize::ParticleElementSize()
	: SizeXMinValue(1.0f)
	, SizeXMaxValue(1.0f)
	, SizeYMinValue(1.0f)
	, SizeYMaxValue(1.0f)
	, ScaleFactorMinValue(1.0f)
	, ScaleFactorMaxValue(1.0f)
{
}

void ParticleElementSize::readData(DataArrayReader& reader)
{
	this->SizeXMinValue = reader.readFloat();
	this->SizeXMaxValue = reader.readFloat();
	this->SizeYMinValue = reader.readFloat();
	this->SizeYMaxValue = reader.readFloat();
	this->ScaleFactorMinValue = reader.readFloat();
	this->ScaleFactorMaxValue = reader.readFloat();
}

void ParticleElementSize::initalizeEffect(SsEffectEmitter* e) const
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
ParticleElementTransSize::ParticleElementTransSize()
	: SizeXMinValue(1.0f)
	, SizeXMaxValue(1.0f)
	, SizeYMinValue(1.0f)
	, SizeYMaxValue(1.0f)
	, ScaleFactorMinValue(1.0f)
	, ScaleFactorMaxValue(1.0f)
{
}

void ParticleElementTransSize::readData(DataArrayReader& reader)
{
	this->SizeXMinValue = reader.readFloat();
	this->SizeXMaxValue = reader.readFloat();
	this->SizeYMinValue = reader.readFloat();
	this->SizeYMaxValue = reader.readFloat();
	this->ScaleFactorMinValue = reader.readFloat();
	this->ScaleFactorMaxValue = reader.readFloat();
}

void ParticleElementTransSize::initalizeEffect(SsEffectEmitter* e) const
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
ParticlePointGravity::ParticlePointGravity()
	: Position(0, 0), Power(0.0f)
{
}

void ParticlePointGravity::readData(DataArrayReader& reader)
{
	this->Position.x = reader.readFloat();
	this->Position.y = reader.readFloat();
	this->Power = reader.readFloat();
}

void ParticlePointGravity::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.usePGravity = true;
	e->particle.gravityPos = this->Position;
//	e->particle.gravityPower = this->Power / 100.0f;
	e->particle.gravityPower = this->Power;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleTurnToDirectionEnabled::ParticleTurnToDirectionEnabled()
	: Rotation(0.0f)
{
}

void ParticleTurnToDirectionEnabled::readData(DataArrayReader& reader)
{
	this->Rotation = reader.readFloat();
}

void ParticleTurnToDirectionEnabled::initalizeEffect(SsEffectEmitter* e) const
{
	e->particle.useTurnDirec = true;
	e->particle.direcRotAdd = this->Rotation;
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
