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





//範囲値クラス
template<class mytype>
class VarianceValue
{
private:

	enum RangeType { None, MinMax, PlusMinus,};

	RangeType 	type;
	mytype 		value;
	mytype 		subvalue;


public:

	VarianceValue( mytype v ){
		value = v;
		subvalue = v;
        type = VarianceValue::None;
	}
	VarianceValue( mytype v , mytype v2){
		value = v;
		subvalue = v2;
        type = VarianceValue::MinMax;
	}

	void	setPlusMinus( mytype v1  )
	{
		value = -v1;
		subvalue = v1;
        type = VarianceValue::PlusMinus;
	}

	void	setMinMax(mytype min , mytype max)
	{
		value = min;
		subvalue = max;
        type = VarianceValue::MinMax;
	}

	mytype	getValue(){ return value;}
	mytype	getMinValue(){ return value;}
	mytype	getMaxValue(){ return subvalue;}

	bool	isTypeNone(){ return  type == VarianceValue::None; }
	bool	isTypeMinMax(){ return  type == VarianceValue::MinMax; }
	bool	isTypePlusMinus(){ return  type == VarianceValue::PlusMinus; }

	mytype* getlpValue(){ return &value;}
	mytype* getlpSubValue(){ return &subvalue;}

	VarianceValue& operator=(mytype v) {  value = v; return *this; }
	operator mytype() { return value; }
};


typedef VarianceValue<float>   			f32VValue;
typedef VarianceValue<int>    			i32VValue;
typedef VarianceValue<SsU8Color>    	SsU8cVValue;




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
	f32VValue	speed;
	i32VValue 	lifespan;
	float		angle;
	float		angleVariance;
	int			interval;
	int			lifetime;
	int			attimeCreate;
    int			priority;

public:
	ParticleElementBasic()
			:	SsEffectElementBase(SsEffectFunctionType::Basic),
				maximumParticle( 50 ),
				speed( 5.0f , 5.0f),
				lifespan( 30 ,30 ),
				angle(0.0f),
				angleVariance(45.0f),
				interval(1),
				lifetime(30),
				attimeCreate(1),
                priority(64)
	{
		//setType( SsEffectFunctionType::Basic );
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
		//setType( SsEffectFunctionType::RndSeedChange );
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
	//	setType( SsEffectFunctionType::Delay );
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
		//setType( SsEffectFunctionType::Gravity );
	}
	virtual ~ParticleElementGravity(){}
};

//--------------------------------------------------------------------------------------
//発生位置への影響
class  ParticleElementPosition : public SsEffectElementBase
{
public:
	f32VValue   OffsetX;
	f32VValue   OffsetY;


	ParticleElementPosition()
		: SsEffectElementBase(SsEffectFunctionType::Position)
		, OffsetX(0,0), OffsetY(0,0)
	{
		//setType( SsEffectFunctionType::Position );
	}
	virtual ~ParticleElementPosition(){}
};


//--------------------------------------------------------------------------------------
//角度変化
class  ParticleElementRotation : public SsEffectElementBase
{
public:
	f32VValue   Rotation;
	f32VValue   RotationAdd;

	ParticleElementRotation()
		: SsEffectElementBase(SsEffectFunctionType::Rotation)
		, Rotation( 0 ,0 )
		, RotationAdd(0,0)
	{
		//setType( SsEffectFunctionType::Rotation );
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
		//setType( SsEffectFunctionType::TransRotation );
	}
	virtual ~ParticleElementRotationTrans(){}
};

//--------------------------------------------------------------------------------------
class  ParticleElementTransSpeed : public SsEffectElementBase
{
public:
	f32VValue	Speed;

	ParticleElementTransSpeed()
        : SsEffectElementBase(SsEffectFunctionType::TransSpeed)
		, Speed( 0,0 )
	{
		//setType( SsEffectFunctionType::TransSpeed );
	}
	virtual ~ParticleElementTransSpeed(){}
};


//--------------------------------------------------------------------------------------
//接戦加速度を与える
class  ParticleElementTangentialAcceleration : public SsEffectElementBase
{
public:
	f32VValue	Acceleration;

	ParticleElementTangentialAcceleration()
		: SsEffectElementBase(SsEffectFunctionType::TangentialAcceleration)
		, Acceleration( 0, 0 )
	{
		//setType( SsEffectFunctionType::TangentialAcceleration );
	}
	virtual ~ParticleElementTangentialAcceleration(){}
};


//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementInitColor : public SsEffectElementBase
{
public:
	SsU8cVValue Color;

	ParticleElementInitColor()
		: SsEffectElementBase(SsEffectFunctionType::InitColor)
		, Color( SsU8Color(255,255,255,255) , SsU8Color(255,255,255,255) )
	{
		//setType( SsEffectFunctionType::InitColor );
	}
	virtual ~ParticleElementInitColor(){}
};

//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementTransColor : public SsEffectElementBase
{
public:
	SsU8cVValue Color;

	ParticleElementTransColor()
		: SsEffectElementBase(SsEffectFunctionType::TransColor)
		, Color( SsU8Color(255,255,255,255) , SsU8Color(255,255,255,255) )
	{
		//setType( SsEffectFunctionType::TransColor );
	}
	virtual ~ParticleElementTransColor(){}
};


//--------------------------------------------------------------------------------------
//著点カラーを制御する
class  ParticleElementAlphaFade : public SsEffectElementBase
{
public:
	f32VValue  disprange;


	ParticleElementAlphaFade()
		: SsEffectElementBase(SsEffectFunctionType::AlphaFade)
		, disprange(25,75)
	{
		//setType( SsEffectFunctionType::AlphaFade );
	}
	virtual ~ParticleElementAlphaFade(){}
};

//--------------------------------------------------------------------------------------
//サイズ初期
class  ParticleElementSize : public SsEffectElementBase
{
public:
	f32VValue SizeX;
	f32VValue SizeY;
	f32VValue ScaleFactor;

	ParticleElementSize()
		: SsEffectElementBase(SsEffectFunctionType::Size)
		, SizeX( 1.0f , 1.0f )
		, SizeY( 1.0f , 1.0f )
		, ScaleFactor( 1.0f , 1.0f )
	{
		//setType( SsEffectFunctionType::Size );
	}
	virtual ~ParticleElementSize(){}

};


//--------------------------------------------------------------------------------------
//サイズ変更
class  ParticleElementTransSize : public SsEffectElementBase
{
public:
	f32VValue SizeX;
	f32VValue SizeY;
	f32VValue ScaleFactor;


	ParticleElementTransSize()
		: SsEffectElementBase(SsEffectFunctionType::TransSize)
		, SizeX( 1.0f , 1.0f )
		, SizeY( 1.0f , 1.0f )
		, ScaleFactor( 1.0f , 1.0f )
	{
		//setType( SsEffectFunctionType::TransSize );
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
