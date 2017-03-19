#ifndef __SSEFFECTELEMENT__
#define __SSEFFECTELEMENT__


#include "sstypes.h"

namespace ss{
class SsEffectEmitter;



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

	virtual void initalizeEffect(SsEffectEmitter* e) = 0;
};



//--------------------------------------------------------------------------------------
//パーティクルを構成する基本の値
class  ParticleElementBasic  : public SsEffectElementBase
{
public:
	int			priority;			//表示優先度
	int			maximumParticle;	//最大パーティクル数
	int			attimeCreate;		//一度に作成するパーティクル数
	int			interval;			//生成間隔
	int			lifetime;			//エミッター生存時間
	float		speedMinValue;		//初速最小
	float		speedMaxValue;		//初速最大
	int 		lifespanMinValue;	//パーティクル生存時間最小
	int 		lifespanMaxValue;	//パーティクル生存時間最大
	float		angle;				//射出方向
	float		angleVariance;		//射出方向範囲


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

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//遅れ
class  ParticleElementRndSeedChange : public SsEffectElementBase
{
public:
	int		Seed;					//上書きするシード値

	ParticleElementRndSeedChange()
		: SsEffectElementBase(SsEffectFunctionType::RndSeedChange)
		, Seed( 0 )
	{
	}
	virtual ~ParticleElementRndSeedChange(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//遅れ
class  ParticleElementDelay : public SsEffectElementBase
{
public:
	int		DelayTime;				//遅延時間

	ParticleElementDelay()
		: SsEffectElementBase(SsEffectFunctionType::Delay)
		, DelayTime( 0 )
	{
	}
	virtual ~ParticleElementDelay(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//重力への影響
class  ParticleElementGravity : public SsEffectElementBase
{
public:

	Vector2   Gravity;				//重力

public:
	ParticleElementGravity()
        : SsEffectElementBase(SsEffectFunctionType::Gravity)
		, Gravity(	0	,	-3.0f	)
    {
	}
	virtual ~ParticleElementGravity(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//発生位置への影響
class  ParticleElementPosition : public SsEffectElementBase
{
public:
	float   OffsetXMinValue;		//X座標に加算最小
	float   OffsetXMaxValue;		//X座標に加算最大
	float   OffsetYMinValue;		//Y座標に加算最小
	float   OffsetYMaxValue;		//Y座標に加算最大


	ParticleElementPosition()
		: SsEffectElementBase(SsEffectFunctionType::Position)
		, OffsetXMinValue(0)
		, OffsetXMaxValue(0)
		, OffsetYMinValue(0)
		, OffsetYMaxValue(0)
	{
	}
	virtual ~ParticleElementPosition(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//角度変化
class  ParticleElementRotation : public SsEffectElementBase
{
public:
	float RotationMinValue;			//角度初期値最小
	float RotationMaxValue;			//角度初期値最大
	float RotationAddMinValue;		//角度初期加算値最小
	float RotationAddMaxValue;		//角度初期加算値最大

	ParticleElementRotation()
		: SsEffectElementBase(SsEffectFunctionType::Rotation)
		, RotationMinValue(0)
		, RotationMaxValue(0)
		, RotationAddMinValue(0)
		, RotationAddMaxValue(0)
	{
	}
	virtual ~ParticleElementRotation(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//角度変化
class  ParticleElementRotationTrans : public SsEffectElementBase
{
public:
	float   RotationFactor;			//角度目標加算値
	float	EndLifeTimePer;			//到達時間

	ParticleElementRotationTrans()
		: SsEffectElementBase(SsEffectFunctionType::TransRotation)
		, RotationFactor( 0 )
		, EndLifeTimePer( 75 )
	{
	}
	virtual ~ParticleElementRotationTrans(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
class  ParticleElementTransSpeed : public SsEffectElementBase
{
public:
	float SpeedMinValue;			//速度目標値最小
	float SpeedMaxValue;			//速度目標値最大

	ParticleElementTransSpeed()
        : SsEffectElementBase(SsEffectFunctionType::TransSpeed)
		, SpeedMinValue(0)
		, SpeedMaxValue(0)
	{
	}
	virtual ~ParticleElementTransSpeed(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//接戦加速度を与える
class  ParticleElementTangentialAcceleration : public SsEffectElementBase
{
public:
	float AccelerationMinValue;		//設定加速度最小
	float AccelerationMaxValue;		//設定加速度最大

	ParticleElementTangentialAcceleration()
		: SsEffectElementBase(SsEffectFunctionType::TangentialAcceleration)
		, AccelerationMinValue(0)
		, AccelerationMaxValue(0)
	{
	}
	virtual ~ParticleElementTangentialAcceleration(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementInitColor : public SsEffectElementBase
{
public:
	SsU8Color ColorMinValue;		//設定カラー最小
	SsU8Color ColorMaxValue;		//設定カラー最大

	ParticleElementInitColor()
		: SsEffectElementBase(SsEffectFunctionType::InitColor)
		, ColorMinValue(255, 255, 255, 255)
		, ColorMaxValue(255, 255, 255, 255)
	{
	}
	virtual ~ParticleElementInitColor(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementTransColor : public SsEffectElementBase
{
public:
	SsU8Color ColorMinValue;		//設定カラー最小
	SsU8Color ColorMaxValue;		//設定カラー最大

	ParticleElementTransColor()
		: SsEffectElementBase(SsEffectFunctionType::TransColor)
		, ColorMinValue(255, 255, 255, 255)
		, ColorMaxValue(255, 255, 255, 255)
	{
	}
	virtual ~ParticleElementTransColor(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//著点カラーを制御する
class  ParticleElementAlphaFade : public SsEffectElementBase
{
public:
	float disprangeMinValue;		//表示区間開始
	float disprangeMaxValue;		//表示区間終了


	ParticleElementAlphaFade()
		: SsEffectElementBase(SsEffectFunctionType::AlphaFade)
		, disprangeMinValue(25)
		, disprangeMaxValue(75)
	{
	}
	virtual ~ParticleElementAlphaFade(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//サイズ初期
class  ParticleElementSize : public SsEffectElementBase
{
public:
	float SizeXMinValue;			//幅倍率最小
	float SizeXMaxValue;			//幅倍率最大
	float SizeYMinValue;			//高さ倍率最小
	float SizeYMaxValue;			//高さ倍率最大
	float ScaleFactorMinValue;		//倍率最小
	float ScaleFactorMaxValue;		//倍率最大


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

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//サイズ変更
class  ParticleElementTransSize : public SsEffectElementBase
{
public:
	float SizeXMinValue;			//幅倍率最小
	float SizeXMaxValue;			//幅倍率最大
	float SizeYMinValue;			//高さ倍率最小
	float SizeYMaxValue;			//高さ倍率最大
	float ScaleFactorMinValue;		//倍率最小
	float ScaleFactorMaxValue;		//倍率最大


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

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//重力点
class  ParticlePointGravity : public SsEffectElementBase
{
public:

	Vector2   Position;				//重力点
	float		Power;				//パワー



	ParticlePointGravity()
		: SsEffectElementBase(SsEffectFunctionType::PointGravity)
		, Position( 0 , 0 ) ,Power(0.0f)
	{
	}
	virtual ~ParticlePointGravity(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};




//--------------------------------------------------------------------------------------
class  ParticleTurnToDirectionEnabled : public SsEffectElementBase
{
public:
	float Rotation;					//方向オフセット

	ParticleTurnToDirectionEnabled()
		: SsEffectElementBase(SsEffectFunctionType::TurnToDirectionEnabled)
		, Rotation(0.0f)
	{
	}
	virtual ~ParticleTurnToDirectionEnabled(){}

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
class  ParticleInfiniteEmitEnabled : public SsEffectElementBase
{
public:
	//int flag;						//ダミーデータ

	ParticleInfiniteEmitEnabled()
		: SsEffectElementBase(SsEffectFunctionType::InfiniteEmitEnabled)
	{
	}
	virtual ~ParticleInfiniteEmitEnabled() {}

	void initalizeEffect(SsEffectEmitter* e) override;
};

};



#endif
