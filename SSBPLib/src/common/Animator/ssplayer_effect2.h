#ifndef __SSEFFECTRENDER_V3__
#define __SSEFFECTRENDER_V3__


#include "xorshift32.h"
#include "../../SS5Player.h"
#include "common/Loader/ssloader_ssee.h"



namespace ss
{

class SsEffectModel;
class SsEffectNode;
class CustomSprite;


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
	float	rot;
	float	direc;

    SsU8Color color;
    Vector2 scale;
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

	SsU8Color   startcolor; //スタート時のカラー
	SsU8Color   endcolor;   //終了時のカラー

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
	SsU8Color   initColor;
	SsU8Color   initColor2;

	bool		useTransColor;
	SsU8Color   transColor;
	SsU8Color   transColor2;

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
    unsigned long*              seedList;


	Vector2   				position;
	SsEffectEmitter*			_parent;

    int							_parentIndex;

	SsEffectBehavior*           refData;	//データ更新用

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
		delete[] seedList;

	}

	void	setSeedOffset(int offset) {
		seedOffset = offset;
	}


	int	getParticleIDMax() { return _offsetPattern.size(); }
	const 	particleExistSt*	getParticleDataFromID(int id);
	void	updateEmitter(double time, int slide);


	int	getTimeLength() { return emitter.life + ( emitter.particleLife + emitter.particleLife2); }

	//現在時間から産出される位置を求める
	//time変数から求められる式とする
	void	updateParticle(float time, particleDrawData* p, bool recalc = false );

	//パーティクルの発生間隔を事前計算する
	//ここで出力が確定する

	void	precalculate2();

};


class SsEffectRenderV2
{
public:

	//エフェクトのパラメータデータ
	SsEffectModel*		effectData;

	//Modelに記載されているエミッタのリスト
	std::vector<SsEffectEmitter*>   emmiterList;

	std::vector<SsEffectEmitter*>   updateList;

	//ランダムシード
	u32				mySeed;

	Vector2		layoutScale;

	float			nowFrame;
	float			targetFrame;
    float			secondNowFrame;

	size_t          effectTimeLength;

    bool			Infinite;	//無限に発生出来るかどうか

	bool			isIntFrame;

	bool			m_isPlay;
	bool			m_isLoop;

	int				seedOffset;
	bool		_isWarningData;

	//親になるスプライト
	CustomSprite						*_parentSprite;

	int			_drawSpritecount; 

public:


protected:
	void 	particleDraw(SsEffectEmitter* e , double t , SsEffectEmitter* parent , particleDrawData* plp, const std::vector<TextuerData>& textures);
	void	initEmitter( SsEffectEmitter* e , SsEffectNode* node);

	void	clearEmitterList();



public:
	SsEffectRenderV2() : effectTimeLength(0), isIntFrame(true), seedOffset(0), mySeed(0), _parentSprite(0){}
	virtual ~SsEffectRenderV2() 
	{
		clearEmitterList();
	}

	virtual void    play(){ m_isPlay=true; }
	virtual void	stop(){ m_isPlay = false;}
	virtual void    pause(){ m_isPlay=false; }
	virtual void	setLoop(bool flag){ m_isLoop = flag; }
	virtual bool	isplay(){return m_isPlay;}
	virtual bool	isloop(){return m_isLoop;}

	virtual void	setEffectData(SsEffectModel* data);


	virtual void	setSeed( u32 seed )
	{
   		mySeed = seed * SEED_MAGIC;
	}


	virtual void	setFrame( float frame )
	{
    	nowFrame = frame;
	}

    virtual float	getFrame(){ return nowFrame; }

	virtual void	update();
	virtual void	draw(const std::vector<TextuerData>& textures);

	virtual void    reload();

    virtual size_t  getEffectTimeLength();

	virtual int	getCurrentFPS();

	bool	getPlayStatus(void){
		return(m_isPlay);
	}

	void	drawSprite(
			int cellIndex, const CellRef* refCell, SsRenderBlendType::_enum blendType,
			Vector2	_position,
			Vector2 _size,
			float     _rotation,
			float	  direction,
			SsFColor	_color,
			const std::vector<TextuerData>& textures	//todo:とりあえず今はこれを渡していくしかない。後で整理する
		);

	void	setSeedOffset(int offset) {
		if (effectData->isLockRandSeed)
		{
			seedOffset = 0;
		}
		else
		{
			seedOffset = offset;
		}
	}
	virtual bool	isInfinity() { return Infinite; }
	virtual bool	isWarning() { return _isWarningData; }

	//親になるスプライトを設定する
	void setParentSprite(CustomSprite* sprite) { _parentSprite = sprite; }

	int	getDrawSpriteCount() { return _drawSpritecount; }
};

};

#endif