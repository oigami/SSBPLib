#include "SsEffectElement.h"
#include "common/Animator/ssplayer_effect2.h"
#include "player/DataArrayReader.h"

namespace ss{


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementBasic::ParticleElementBasic()
	: SsEffectElementBase(SsEffectFunctionType::Basic)
	, priority(64)
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
	EffectParticleElementBasic readparam;
	readparam.priority = reader.readU32();			//表示優先度
	readparam.maximumParticle = reader.readU32();		//最大パーティクル数
	readparam.attimeCreate = reader.readU32();		//一度に作成するパーティクル数
	readparam.interval = reader.readU32();			//生成間隔
	readparam.lifetime = reader.readU32();			//エミッター生存時間
	readparam.speedMinValue = reader.readFloat();		//初速最小
	readparam.speedMaxValue = reader.readFloat();		//初速最大
	readparam.lifespanMinValue = reader.readU32();	//パーティクル生存時間最小
	readparam.lifespanMaxValue = reader.readU32();	//パーティクル生存時間最大
	readparam.angle = reader.readFloat();				//射出方向
	readparam.angleVariance = reader.readFloat();		//射出方向範囲

	this->priority = readparam.priority;							//表示優先度
	this->maximumParticle = readparam.maximumParticle;			//最大パーティクル数
	this->attimeCreate = readparam.attimeCreate;					//一度に作成するパーティクル数
	this->interval = readparam.interval;							//生成間隔
	this->lifetime = readparam.lifetime;							//エミッター生存時間
	this->speedMinValue = readparam.speedMinValue;				//初速
	this->speedMaxValue = readparam.speedMaxValue;				//初速
	this->lifespanMinValue = readparam.lifespanMinValue;			//パーティクル生存時間
	this->lifespanMaxValue = readparam.lifespanMaxValue;			//パーティクル生存時間
	this->angle = readparam.angle;								//射出方向
	this->angleVariance = readparam.angleVariance;				//射出方向範囲
}

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
ParticleElementRndSeedChange::ParticleElementRndSeedChange()
	: SsEffectElementBase(SsEffectFunctionType::RndSeedChange)
	, Seed(0)
{
}

void ParticleElementRndSeedChange::readData(DataArrayReader& reader)
{
	EffectParticleElementRndSeedChange readparam;
	readparam.Seed = reader.readU32();				//上書きするシード値

	this->Seed = readparam.Seed;							//上書きするシード値
}

void ParticleElementRndSeedChange::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.userOverrideRSeed = true;

	e->particle.overrideRSeed = this->Seed + SEED_MAGIC;
	e->emitterSeed = this->Seed + SEED_MAGIC;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementDelay::ParticleElementDelay()
	: SsEffectElementBase(SsEffectFunctionType::Delay)
	, DelayTime(0)
{
}

void ParticleElementDelay::readData(DataArrayReader& reader)
{
	EffectParticleElementDelay readparam;
	readparam.DelayTime = reader.readU32();			//遅延時間

	this->DelayTime = readparam.DelayTime;			//遅延時間
}

void ParticleElementDelay::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.delay = this->DelayTime;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementGravity::ParticleElementGravity()
	: SsEffectElementBase(SsEffectFunctionType::Gravity)
	, Gravity(0, -3.0f)
{
}

void ParticleElementGravity::readData(DataArrayReader& reader)
{
	EffectParticleElementGravity readparam;
	readparam.Gravity_x = reader.readFloat();			//X方向の重力
	readparam.Gravity_y = reader.readFloat();			//Y方向の重力

	this->Gravity.x = readparam.Gravity_x;			//X方向の重力
	this->Gravity.y = readparam.Gravity_y;			//Y方向の重力
}

void ParticleElementGravity::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useGravity = true;
	e->particle.gravity = this->Gravity;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementPosition::ParticleElementPosition()
	: SsEffectElementBase(SsEffectFunctionType::Position)
	, OffsetXMinValue(0)
	, OffsetXMaxValue(0)
	, OffsetYMinValue(0)
	, OffsetYMaxValue(0)
{
}

void ParticleElementPosition::readData(DataArrayReader& reader)
{
	EffectParticleElementPosition readparam;
	readparam.OffsetXMinValue = reader.readFloat();	//X座標に加算最小
	readparam.OffsetXMaxValue = reader.readFloat();	//X座標に加算最大
	readparam.OffsetYMinValue = reader.readFloat();	//Y座標に加算最小
	readparam.OffsetYMaxValue = reader.readFloat();	//Y座標に加算最大

	this->OffsetXMinValue = readparam.OffsetXMinValue; 	//X座標に加算最小
	this->OffsetXMaxValue = readparam.OffsetXMaxValue; 	//X座標に加算最大
	this->OffsetYMinValue = readparam.OffsetYMinValue;	//Y座標に加算最小
	this->OffsetYMaxValue = readparam.OffsetYMaxValue;	//Y座標に加算最大
}

void ParticleElementPosition::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useOffset = true;
	e->particle.offset = Vector2(this->OffsetXMinValue, this->OffsetYMinValue);
	e->particle.offset2 = Vector2(this->OffsetXMaxValue - this->OffsetXMinValue, this->OffsetYMaxValue - this->OffsetYMinValue);
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementRotation::ParticleElementRotation()
	: SsEffectElementBase(SsEffectFunctionType::Rotation)
	, RotationMinValue(0)
	, RotationMaxValue(0)
	, RotationAddMinValue(0)
	, RotationAddMaxValue(0)
{
}

void ParticleElementRotation::readData(DataArrayReader& reader)
{
	EffectParticleElementRotation readparam;
	readparam.RotationMinValue = reader.readFloat();		//角度初期値最小
	readparam.RotationMaxValue = reader.readFloat();		//角度初期値最大
	readparam.RotationAddMinValue = reader.readFloat();	//角度初期加算値最小
	readparam.RotationAddMaxValue = reader.readFloat();	//角度初期加算値最大

	this->RotationMinValue = readparam.RotationMinValue;		//角度初期値最小
	this->RotationMaxValue = readparam.RotationMaxValue;		//角度初期値最大
	this->RotationAddMinValue = readparam.RotationAddMinValue;	//角度初期加算値最小
	this->RotationAddMaxValue = readparam.RotationAddMaxValue;	//角度初期加算値最大
}

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
ParticleElementRotationTrans::ParticleElementRotationTrans()
	: SsEffectElementBase(SsEffectFunctionType::TransRotation)
	, RotationFactor(0)
	, EndLifeTimePer(75)
{
}

void ParticleElementRotationTrans::readData(DataArrayReader& reader)
{
	EffectParticleElementRotationTrans readparam;
	readparam.RotationFactor = reader.readFloat();		//角度目標加算値
	readparam.EndLifeTimePer = reader.readFloat();		//到達時間

	this->RotationFactor = readparam.RotationFactor;		//角度目標加算値
	this->EndLifeTimePer = readparam.EndLifeTimePer;		//到達時間
}

void ParticleElementRotationTrans::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useRotationTrans = true;
	e->particle.rotationFactor = this->RotationFactor;
	e->particle.endLifeTimePer = this->EndLifeTimePer / 100.0f;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTransSpeed::ParticleElementTransSpeed()
	: SsEffectElementBase(SsEffectFunctionType::TransSpeed)
	, SpeedMinValue(0)
	, SpeedMaxValue(0)
{
}

void ParticleElementTransSpeed::readData(DataArrayReader& reader)
{
	EffectParticleElementTransSpeed readparam;
	readparam.SpeedMinValue = reader.readFloat();			//速度目標値最小
	readparam.SpeedMaxValue = reader.readFloat();			//速度目標値最大

	this->SpeedMinValue = readparam.SpeedMinValue;			//速度目標値最小
	this->SpeedMaxValue = readparam.SpeedMaxValue;			//速度目標値最大
}

void ParticleElementTransSpeed::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useTransSpeed = true;
	e->particle.transSpeed = this->SpeedMinValue;
	e->particle.transSpeed2 = this->SpeedMaxValue - this->SpeedMinValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementTangentialAcceleration::ParticleElementTangentialAcceleration()
	: SsEffectElementBase(SsEffectFunctionType::TangentialAcceleration)
	, AccelerationMinValue(0)
	, AccelerationMaxValue(0)
{
}

void ParticleElementTangentialAcceleration::readData(DataArrayReader& reader)
{
	EffectParticleElementTangentialAcceleration readparam;
	readparam.AccelerationMinValue = reader.readFloat();	//設定加速度最小
	readparam.AccelerationMaxValue = reader.readFloat();	//設定加速度最大

	this->AccelerationMinValue = readparam.AccelerationMinValue;	//設定加速度最小
	this->AccelerationMaxValue = readparam.AccelerationMaxValue;	//設定加速度最大
}

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
ParticleElementInitColor::ParticleElementInitColor()
	: SsEffectElementBase(SsEffectFunctionType::InitColor)
	, ColorMinValue(255, 255, 255, 255)
	, ColorMaxValue(255, 255, 255, 255)
{
}

void ParticleElementInitColor::readData(DataArrayReader& reader)
{
	EffectParticleElementInitColor readparam;
	readparam.ColorMinValue = reader.readU32();			//設定カラー最小
	readparam.ColorMaxValue = reader.readU32();			//設定カラー最大

	int a = (readparam.ColorMinValue & 0xFF000000) >> 24;
	int r = (readparam.ColorMinValue & 0x00FF0000) >> 16;
	int g = (readparam.ColorMinValue & 0x0000FF00) >> 8;
	int b = (readparam.ColorMinValue & 0x000000FF) >> 0;
	SsU8Color mincol(r, g, b, a);
	a = (readparam.ColorMaxValue & 0xFF000000) >> 24;
	r = (readparam.ColorMaxValue & 0x00FF0000) >> 16;
	g = (readparam.ColorMaxValue & 0x0000FF00) >> 8;
	b = (readparam.ColorMaxValue & 0x000000FF) >> 0;
	SsU8Color maxcol(r, g, b, a);
	this->ColorMinValue = mincol;			//設定カラー最小
	this->ColorMaxValue = maxcol;			//設定カラー最大
}

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
ParticleElementTransColor::ParticleElementTransColor()
	: SsEffectElementBase(SsEffectFunctionType::TransColor)
	, ColorMinValue(255, 255, 255, 255)
	, ColorMaxValue(255, 255, 255, 255)
{
}

void ParticleElementTransColor::readData(DataArrayReader& reader)
{
	EffectParticleElementTransColor readparam;
	readparam.ColorMinValue = reader.readU32();			//設定カラー最小
	readparam.ColorMaxValue = reader.readU32();			//設定カラー最大

	int a = (readparam.ColorMinValue & 0xFF000000) >> 24;
	int r = (readparam.ColorMinValue & 0x00FF0000) >> 16;
	int g = (readparam.ColorMinValue & 0x0000FF00) >> 8;
	int b = (readparam.ColorMinValue & 0x000000FF) >> 0;
	SsU8Color mincol(r, g, b, a);
	a = (readparam.ColorMaxValue & 0xFF000000) >> 24;
	r = (readparam.ColorMaxValue & 0x00FF0000) >> 16;
	g = (readparam.ColorMaxValue & 0x0000FF00) >> 8;
	b = (readparam.ColorMaxValue & 0x000000FF) >> 0;
	SsU8Color maxcol(r, g, b, a);
	this->ColorMinValue = mincol;			//設定カラー最小
	this->ColorMaxValue = maxcol;			//設定カラー最大
}

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
ParticleElementAlphaFade::ParticleElementAlphaFade()
	: SsEffectElementBase(SsEffectFunctionType::AlphaFade)
	, disprangeMinValue(25)
	, disprangeMaxValue(75)
{
}

void ParticleElementAlphaFade::readData(DataArrayReader& reader)
{
	EffectParticleElementAlphaFade readparam;
	readparam.disprangeMinValue = reader.readFloat();		//表示区間開始
	readparam.disprangeMaxValue = reader.readFloat();		//表示区間終了

	this->disprangeMinValue = readparam.disprangeMinValue;		//表示区間開始
	this->disprangeMaxValue = readparam.disprangeMaxValue;		//表示区間終了
}

void ParticleElementAlphaFade::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useAlphaFade = true;
	e->particle.alphaFade = this->disprangeMinValue;
	e->particle.alphaFade2 = this->disprangeMaxValue;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleElementSize::ParticleElementSize()
	: SsEffectElementBase(SsEffectFunctionType::Size)
	, SizeXMinValue(1.0f)
	, SizeXMaxValue(1.0f)
	, SizeYMinValue(1.0f)
	, SizeYMaxValue(1.0f)
	, ScaleFactorMinValue(1.0f)
	, ScaleFactorMaxValue(1.0f)
{
}

void ParticleElementSize::readData(DataArrayReader& reader)
{
	EffectParticleElementSize readparam;
	readparam.SizeXMinValue = reader.readFloat();			//幅倍率最小
	readparam.SizeXMaxValue = reader.readFloat();			//幅倍率最大
	readparam.SizeYMinValue = reader.readFloat();			//高さ倍率最小
	readparam.SizeYMaxValue = reader.readFloat();			//高さ倍率最大
	readparam.ScaleFactorMinValue = reader.readFloat();		//倍率最小
	readparam.ScaleFactorMaxValue = reader.readFloat();		//倍率最大

	this->SizeXMinValue = readparam.SizeXMinValue;			//幅倍率最小
	this->SizeXMaxValue = readparam.SizeXMaxValue;			//幅倍率最大
	this->SizeYMinValue = readparam.SizeYMinValue;			//高さ倍率最小
	this->SizeYMaxValue = readparam.SizeYMaxValue;			//高さ倍率最大
	this->ScaleFactorMinValue = readparam.ScaleFactorMinValue;		//倍率最小
	this->ScaleFactorMaxValue = readparam.ScaleFactorMaxValue;		//倍率最大
}

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
ParticleElementTransSize::ParticleElementTransSize()
	: SsEffectElementBase(SsEffectFunctionType::TransSize)
	, SizeXMinValue(1.0f)
	, SizeXMaxValue(1.0f)
	, SizeYMinValue(1.0f)
	, SizeYMaxValue(1.0f)
	, ScaleFactorMinValue(1.0f)
	, ScaleFactorMaxValue(1.0f)
{
}

void ParticleElementTransSize::readData(DataArrayReader& reader)
{
	EffectParticleElementTransSize readparam;
	readparam.SizeXMinValue = reader.readFloat();			//幅倍率最小
	readparam.SizeXMaxValue = reader.readFloat();			//幅倍率最大
	readparam.SizeYMinValue = reader.readFloat();			//高さ倍率最小
	readparam.SizeYMaxValue = reader.readFloat();			//高さ倍率最大
	readparam.ScaleFactorMinValue = reader.readFloat();		//倍率最小
	readparam.ScaleFactorMaxValue = reader.readFloat();		//倍率最大

	this->SizeXMinValue = readparam.SizeXMinValue;			//幅倍率最小
	this->SizeXMaxValue = readparam.SizeXMaxValue;			//幅倍率最大
	this->SizeYMinValue = readparam.SizeYMinValue;			//高さ倍率最小
	this->SizeYMaxValue = readparam.SizeYMaxValue;			//高さ倍率最大
	this->ScaleFactorMinValue = readparam.ScaleFactorMinValue;		//倍率最小
	this->ScaleFactorMaxValue = readparam.ScaleFactorMaxValue;		//倍率最大
}

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
ParticlePointGravity::ParticlePointGravity()
	: SsEffectElementBase(SsEffectFunctionType::PointGravity)
	, Position(0, 0), Power(0.0f)
{
}

void ParticlePointGravity::readData(DataArrayReader& reader)
{
	EffectParticlePointGravity readparam;
	readparam.Position_x = reader.readFloat();				//重力点X
	readparam.Position_y = reader.readFloat();				//重力点Y
	readparam.Power = reader.readFloat();					//パワー

	this->Position.x = readparam.Position_x;				//重力点X
	this->Position.y = readparam.Position_y;				//重力点Y
	this->Power = readparam.Power;					//パワー
}

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
ParticleTurnToDirectionEnabled::ParticleTurnToDirectionEnabled()
	: SsEffectElementBase(SsEffectFunctionType::TurnToDirectionEnabled)
	, Rotation(0.0f)
{
}

void ParticleTurnToDirectionEnabled::readData(DataArrayReader& reader)
{
	EffectParticleTurnToDirectionEnabled readparam;
	readparam.Rotation = reader.readFloat();					//フラグ

	this->Rotation = readparam.Rotation;
}

void ParticleTurnToDirectionEnabled::initalizeEffect(SsEffectEmitter* e)
{
	e->particle.useTurnDirec = true;
	e->particle.direcRotAdd = this->Rotation;
}


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
ParticleInfiniteEmitEnabled::ParticleInfiniteEmitEnabled()
	: SsEffectElementBase(SsEffectFunctionType::InfiniteEmitEnabled)
{
}

void ParticleInfiniteEmitEnabled::readData(DataArrayReader& reader)
{
	EffectParticleInfiniteEmitEnabled readparam;
	readparam.flag = reader.readS32();					//ダミーデータが入っているので読み込む必要がある
}

void ParticleInfiniteEmitEnabled::initalizeEffect(SsEffectEmitter* e)
{
	e->emitter.Infinite = true;
}



} //namspace ss
