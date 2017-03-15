#ifndef __SSEFFECTELEMENT__
#define __SSEFFECTELEMENT__


#include "sstypes.h"

namespace ss
{

//命令種別
namespace SsEffectFunctionType
{
	enum enum_
	{
		Base,
		Basic	,
		RndSeedChange , 
		Delay,
		Gravity,
		Position,
		//TransPosition,
		Rotation,
		TransRotation,
		TransSpeed,
		TangentialAcceleration,
		InitColor,
		TransColor,
		AlphaFade,
		Size,
		TransSize,
		PointGravity,
		TurnToDirectionEnabled,
		InfiniteEmitEnabled,
	};
}



class SsEffectRenderEmitter;
class SsEffectRenderParticle;


class   SsEffectElementBase 
{
private:
	SsEffectFunctionType::enum_ myType;

public:
	SsEffectElementBase(SsEffectFunctionType::enum_ functionType)
		: myType (functionType) {}
	virtual ~SsEffectElementBase(){}

	SsEffectFunctionType::enum_ getFunctionType() const{ return myType; }
//	void	setType(SsEffectFunctionType::enum_ type){ myType = type; } 

#if 0
	//各部で実装する
	virtual void InitializeEmmiter( SsEffectRenderEmitter* emmiter ) {}
	virtual void UpdateEmmiter( SsEffectRenderEmitter* emmiter ){}
	virtual void UpdateEndEmmiter( SsEffectRenderEmitter* emmiter ){}
	virtual void InitializeParticle( SsEffectRenderEmitter* e , SsEffectRenderParticle* particle ){}
	virtual void UpdateParticle( SsEffectRenderParticle* particle ){}
#endif
};



//--------------------------------------------------------------------------------------
//パーティクルを構成する基本の値
class  ParticleElementBasic  : public SsEffectElementBase
{
public:
	int			maximumParticle;
	float		speedMinValue;
	float		speedMaxValue;
	int 		lifespanMinValue;
	int 		lifespanMaxValue;
	float		angle;
	float		angleVariance;
	int			interval;
	int			lifetime;
	int			attimeCreate;
    int			priority;

public:
	ParticleElementBasic()
		: SsEffectElementBase(SsEffectFunctionType::Basic)
		, maximumParticle( 50 )
		, speedMinValue( 5.0f )
		, speedMaxValue( 5.0f)
		, lifespanMinValue( 30 )
		, lifespanMaxValue( 30 )
		, angle(0.0f)
		, angleVariance(45.0f)
		, interval(1)
		, lifetime(30)
		, attimeCreate(1)
		, priority(64)
	{
	}

	virtual ~ParticleElementBasic(){}
};


//--------------------------------------------------------------------------------------
//遅れ
class  ParticleElementRndSeedChange : public SsEffectElementBase
{
public:
	int		Seed;

	ParticleElementRndSeedChange()
		: SsEffectElementBase(SsEffectFunctionType::RndSeedChange)
		, Seed( 0 )
	{
	}
	virtual ~ParticleElementRndSeedChange(){}
};

//--------------------------------------------------------------------------------------
//遅れ
class  ParticleElementDelay : public SsEffectElementBase
{
public:
	int		DelayTime;

	ParticleElementDelay()
		: SsEffectElementBase(SsEffectFunctionType::Delay)
		, DelayTime( 0 )
	{
	}
	virtual ~ParticleElementDelay(){}
};


//--------------------------------------------------------------------------------------
//重力への影響
class  ParticleElementGravity : public SsEffectElementBase
{
public:

	Vector2   Gravity;

public:
	ParticleElementGravity()
        : SsEffectElementBase(SsEffectFunctionType::Gravity)
		, Gravity(	0	,	-3.0f	)
    {
	}
	virtual ~ParticleElementGravity(){}
};

//--------------------------------------------------------------------------------------
//発生位置への影響
class  ParticleElementPosition : public SsEffectElementBase
{
public:
	float   OffsetXMinValue;
	float   OffsetXMaxValue;
	float   OffsetYMinValue;
	float   OffsetYMaxValue;


	ParticleElementPosition()
		: SsEffectElementBase(SsEffectFunctionType::Position)
		, OffsetXMinValue(0)
		, OffsetXMaxValue(0)
		, OffsetYMinValue(0)
		, OffsetYMaxValue(0)
	{
	}
	virtual ~ParticleElementPosition(){}
};


//--------------------------------------------------------------------------------------
//角度変化
class  ParticleElementRotation : public SsEffectElementBase
{
public:
	float RotationMinValue;
	float RotationMaxValue;
	float RotationAddMinValue;
	float RotationAddMaxValue;

	ParticleElementRotation()
		: SsEffectElementBase(SsEffectFunctionType::Rotation)
		, RotationMinValue(0)
		, RotationMaxValue(0)
		, RotationAddMinValue(0)
		, RotationAddMaxValue(0)
	{
	}
	virtual ~ParticleElementRotation(){}
};

//--------------------------------------------------------------------------------------
//角度変化
class  ParticleElementRotationTrans : public SsEffectElementBase
{
public:
	float   RotationFactor;
	float	EndLifeTimePer;

	ParticleElementRotationTrans()
		: SsEffectElementBase(SsEffectFunctionType::TransRotation)
		, RotationFactor( 0 )
		, EndLifeTimePer( 75 )
	{
	}
	virtual ~ParticleElementRotationTrans(){}
};

//--------------------------------------------------------------------------------------
class  ParticleElementTransSpeed : public SsEffectElementBase
{
public:
	float SpeedMinValue;
	float SpeedMaxValue;

	ParticleElementTransSpeed()
        : SsEffectElementBase(SsEffectFunctionType::TransSpeed)
		, SpeedMinValue(0)
		, SpeedMaxValue(0)
	{
	}
	virtual ~ParticleElementTransSpeed(){}
};


//--------------------------------------------------------------------------------------
//接戦加速度を与える
class  ParticleElementTangentialAcceleration : public SsEffectElementBase
{
public:
	float AccelerationMinValue;
	float AccelerationMaxValue;

	ParticleElementTangentialAcceleration()
		: SsEffectElementBase(SsEffectFunctionType::TangentialAcceleration)
		, AccelerationMinValue(0)
		, AccelerationMaxValue(0)
	{
	}
	virtual ~ParticleElementTangentialAcceleration(){}
};


//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementInitColor : public SsEffectElementBase
{
public:
	SsU8Color ColorMinValue;
	SsU8Color ColorMaxValue;

	ParticleElementInitColor()
		: SsEffectElementBase(SsEffectFunctionType::InitColor)
		, ColorMinValue(255, 255, 255, 255)
		, ColorMaxValue(255, 255, 255, 255)
	{
	}
	virtual ~ParticleElementInitColor(){}
};

//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementTransColor : public SsEffectElementBase
{
public:
	SsU8Color ColorMinValue;
	SsU8Color ColorMaxValue;

	ParticleElementTransColor()
		: SsEffectElementBase(SsEffectFunctionType::TransColor)
		, ColorMinValue(255, 255, 255, 255)
		, ColorMaxValue(255, 255, 255, 255)
	{
	}
	virtual ~ParticleElementTransColor(){}
};


//--------------------------------------------------------------------------------------
//著点カラーを制御する
class  ParticleElementAlphaFade : public SsEffectElementBase
{
public:
	float disprangeMinValue;
	float disprangeMaxValue;


	ParticleElementAlphaFade()
		: SsEffectElementBase(SsEffectFunctionType::AlphaFade)
		, disprangeMinValue(25)
		, disprangeMaxValue(75)
	{
	}
	virtual ~ParticleElementAlphaFade(){}
};

//--------------------------------------------------------------------------------------
//サイズ初期
class  ParticleElementSize : public SsEffectElementBase
{
public:
	float SizeXMinValue;
	float SizeXMaxValue;
	float SizeYMinValue;
	float SizeYMaxValue;
	float ScaleFactorMinValue;
	float ScaleFactorMaxValue;


	ParticleElementSize()
		: SsEffectElementBase(SsEffectFunctionType::Size)
		, SizeXMinValue(1.0f)
		, SizeXMaxValue(1.0f)
		, SizeYMinValue(1.0f)
		, SizeYMaxValue(1.0f)
		, ScaleFactorMinValue(1.0f)
		, ScaleFactorMaxValue(1.0f)
	{
	}
	virtual ~ParticleElementSize(){}

};


//--------------------------------------------------------------------------------------
//サイズ変更
class  ParticleElementTransSize : public SsEffectElementBase
{
public:
	float SizeXMinValue;
	float SizeXMaxValue;
	float SizeYMinValue;
	float SizeYMaxValue;
	float ScaleFactorMinValue;
	float ScaleFactorMaxValue;


	ParticleElementTransSize()
		: SsEffectElementBase(SsEffectFunctionType::TransSize)
		, SizeXMinValue(1.0f)
		, SizeXMaxValue(1.0f)
		, SizeYMinValue(1.0f)
		, SizeYMaxValue(1.0f)
		, ScaleFactorMinValue(1.0f)
		, ScaleFactorMaxValue(1.0f)
	{
	}
	virtual ~ParticleElementTransSize(){}
};

//--------------------------------------------------------------------------------------
//重力点
class  ParticlePointGravity : public SsEffectElementBase
{
public:

	Vector2   Position;
    float		Power;



	ParticlePointGravity()
		: SsEffectElementBase(SsEffectFunctionType::PointGravity)
		, Position( 0 , 0 ) ,Power(0.0f)
	{
		//setType(SsEffectFunctionType::PointGravity);
	}
	virtual ~ParticlePointGravity(){}

};




//--------------------------------------------------------------------------------------
class  ParticleTurnToDirectionEnabled : public SsEffectElementBase
{
public:
	float Rotation;

	ParticleTurnToDirectionEnabled()
		: SsEffectElementBase(SsEffectFunctionType::TurnToDirectionEnabled)
		, Rotation(0.0f)
	{
		//setType( SsEffectFunctionType::TurnToDirectionEnabled );
	}
	virtual ~ParticleTurnToDirectionEnabled(){}
};


//--------------------------------------------------------------------------------------
class  ParticleInfiniteEmitEnabled : public SsEffectElementBase
{
public:

	ParticleInfiniteEmitEnabled()
		: SsEffectElementBase(SsEffectFunctionType::InfiniteEmitEnabled)
	{
		//setType(SsEffectFunctionType::InfiniteEmitEnabled);
	}
	virtual ~ParticleInfiniteEmitEnabled() {}
};

};



#endif
