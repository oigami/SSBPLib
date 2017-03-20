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
	const SsEffectModel*		effectData;

	//Modelに記載されているエミッタのリスト
	std::vector<SsEffectEmitter*>   emmiterList;
	std::vector<SsEffectEmitter*>   updateList;

	//ランダムシード
	int				mySeed;

	float			nowFrame;
	float			targetFrame;

	size_t          effectTimeLength;

    bool			Infinite;	//無限に発生出来るかどうか

	bool			m_isPlay;
	bool			m_isLoop;

	int				seedOffset;
	bool		_isWarningData;

	//親になるスプライト
	CustomSprite						*_parentSprite;

	int			_drawSpritecount; 


private:
	void 	particleDraw(SsEffectEmitter* e , double t , SsEffectEmitter* parent , particleDrawData* plp, const std::vector<TextuerData>& textures);
	void	initEmitter( SsEffectEmitter* e , const SsEffectNode* node);


public:
	SsEffectRenderV2(const SsEffectModel* model, int seed);
	~SsEffectRenderV2();

	void    play(){ m_isPlay=true; }
	void	stop(){ m_isPlay = false;}
	void	setLoop(bool flag){ m_isLoop = flag; }
	bool	isplay(){return m_isPlay;}
	bool	isloop(){return m_isLoop;}

	void	setFrame( float frame ){
    	nowFrame = frame;
	}
    float	getFrame(){ return nowFrame; }

	void	update();
	void	draw(const std::vector<TextuerData>& textures);

private:
	void    initialize();

public:
    size_t  getEffectTimeLength();

#if 0
	int	getCurrentFPS();
#endif

	void	drawSprite(
			int cellIndex, const CellRef* refCell, SsRenderBlendType blendType,
			Vector2	_position,
			Vector2 _size,
			float     _rotation,
			float	  direction,
			SsFColor	_color,
			const std::vector<TextuerData>& textures	//todo:とりあえず今はこれを渡していくしかない。後で整理する
		);

	void	setSeedOffset(int offset) {
		if (effectData->isLockRandSeed())
		{
			seedOffset = 0;
		}
		else
		{
			seedOffset = offset;
		}
	}
	bool	isInfinity() { return Infinite; }
	bool	isWarning() { return _isWarningData; }

	//親になるスプライトを設定する
	void setParentSprite(CustomSprite* sprite) { _parentSprite = sprite; }

	int	getDrawSpriteCount() { return _drawSpritecount; }
};

};

#endif