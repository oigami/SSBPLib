#pragma once
#include "xorshift32.h"
#include "effect/ssloader_ssee.h"
#include "math/Matrix.h"


namespace ss{
class SsEffectNode;



#define SEED_MAGIC (7573)
#define LIFE_EXTEND_SCALE (8)
#define LIFE_EXTEND_MIN	  (64)



//v3.1
struct particleExistSt
{
	int  id;
	int	 cycle;
	int	 exist;
    int	 born;
	long stime;
	long endtime;
};


//v3.1
struct emitPattern
{
	int	  uid;
	int   life;
    int   cycle;
};


//最終描画用データ
struct particleDrawData
{
	int	id;
	int	pid;
	int	stime;		//生成された開始時間
	int lifetime;

	//描画用ワーク
	float	x;
	float	y;
	float	rot;	//degree
	float	direc;	//radian

    SSColor4B color;
    Vector2 scale;

	//変換行列の取得(layoutScaleは一応そのまま持ってきたがそもそもこれで合ってるのか謎)
	Matrix craeteLocalTransformMatrix(Vector2 layoutScale) const{
		Matrix localTransformMatrix;
		localTransformMatrix.setupSRzyxT(
			Vector3(scale.x, scale.y, 1.0f),
			Vector3(0.0f, 0.0f, SSDegToRad(rot) + direc),
			Vector3(x * layoutScale.x, y * layoutScale.y, 0.0f)
		);
		return localTransformMatrix;
	}
};


//エミッターが持つパラメータ
//エディタやファイルから入力される
struct emitterParameter
{
	int	life;
	int interval;
	int	emitnum;		//一度に排出される個数
	int emitmax;		//最大排出数
	int	particleLife;	//生成されるパーティクルの寿命
	int particleLife2;	//寿命最大値
    bool Infinite;      //無限発生

	int	loopStart;
	int loopEnd;
	int loopLen;
	int	loopGen;


	//テスト用デフォルト
	emitterParameter() : life(15),interval(1),emitnum(2),emitmax(32),particleLife(15),particleLife2(15),Infinite(false){}

};


//パーティクルが持つパラメータ
//エディタやファイルから入力される
struct particleParameter
{

	Vector2 	scale;

	SSColor4B   startcolor; //スタート時のカラー
	SSColor4B   endcolor;   //終了時のカラー

	//初速
	float		speed;		//初速
	float		speed2;		//初速最大値


	float		angle;		  //向いてる角度
	float       angleVariance;//変差

	bool		useGravity;
	Vector2	gravity;


	bool		useOffset;
	Vector2   offset;
	Vector2   offset2;

	bool        useRotation;
	float		rotation;
	float		rotation2;

	float		rotationAdd;
	float		rotationAdd2;

	bool		useRotationTrans;
	float		rotationFactor;
	float		endLifeTimePer;

	bool        useTanAccel;
	float		tangentialAccel;
	float		tangentialAccel2;

	bool        useColor;
	SSColor4B   initColor;
	SSColor4B   initColor2;

	bool		useTransColor;
	SSColor4B   transColor;
	SSColor4B   transColor2;

	bool        useInitScale;
	Vector2   scaleRange;
	float		scaleFactor;
	float		scaleFactor2;

	bool        useTransScale;
	Vector2   transscale;
	Vector2   transscaleRange;
	float		transscaleFactor;
	float		transscaleFactor2;

	float		delay;

	bool		usePGravity;
	Vector2	gravityPos;
	float		gravityPower;

	bool		useAlphaFade;
	float       alphaFade;
	float       alphaFade2;

	bool		useTransSpeed;
	float	   	transSpeed;
	float	   	transSpeed2;

	bool		useTurnDirec;
    float		direcRotAdd;

    bool		userOverrideRSeed;
	int			overrideRSeed;


	particleParameter(){}
};


//エミッタ動作クラス
class SsEffectEmitter
{
public:
	int					priority;

	emitterParameter  	emitter;
	particleParameter   particle;
	xorshift32			rand;


	int					emitterSeed;
	int					seedOffset;

	//生成用のリングバッファ
	std::vector<emitPattern>    	_emitpattern;
	std::vector<int>				_offsetPattern;

    particleExistSt*     particleExistList;


	//事前計算バッファ
	int							particleIdMax;

	size_t						particleListBufferSize;
    std::vector<unsigned long>  seedList;


	Vector2   				position;
	SsEffectEmitter*			_parent;

    int							_parentIndex;

	const SsEffectNode*     refData;	//データ更新用

	size_t						globaltime;
	size_t						seedTableLen;

	int							uid; 

public:
	SsEffectEmitter() :
//			particleList(0),
			_parentIndex(-1),
			seedList(0),
			particleListBufferSize(180*100),  //生成出来るパーティクルの最大値
			_emitpattern(0),
			particleExistList(0),
			globaltime(0),
			seedOffset(0)
	{
		emitterSeed = SEED_MAGIC;
	}
	virtual ~SsEffectEmitter()
	{
		delete[] particleExistList;
	}

	void	setSeedOffset(int offset) {
		seedOffset = offset;
	}


	int	getParticleIDMax()const { return _offsetPattern.size(); }
	const 	particleExistSt*	getParticleDataFromID(int id) const;
	void	updateEmitter(double time, int slide);


	int	getTimeLength() const{
		return emitter.life + emitter.particleLife + emitter.particleLife2;
	}

	//現在時間から産出される位置を求める
	//time変数から求められる式とする
	void	updateParticle(float time, particleDrawData* p, bool recalc = false );

	//パーティクルの発生間隔を事前計算する
	//ここで出力が確定する

	void	precalculate2();

};



} //namespace ss
