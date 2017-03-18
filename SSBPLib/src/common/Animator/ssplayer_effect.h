#ifndef __SSPLAYER_EFFECT__
#define __SSPLAYER_EFFECT__

#include <list>
#include "../../SS5Player.h"
#include "MersenneTwister.h"
#include "common/Loader/ssloader_ssee.h"

namespace ss
{

class SsEffectNode;
class SsEffectBehavior;



//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
class SsEffectRenderAtom
{
public:
	Vector3  			position;
	float				rotation;
	Vector2	  		scale;

	SsEffectRenderAtom*	parent;
	SsEffectNode*		data;
    bool				m_isLive;
	bool				m_isInit;
    bool				m_isCreateChild;

	float				_lifetime;		//オブジェクトの最大生存時間
	float				_exsitTime;		//存在した時間
	float				_life;          //寿命 = 0で死

	bool                undead;

	float				alpha;

public:

	SsEffectRenderAtom() :
		parent(0),
		m_isInit(false),
		m_isLive(true),
		_lifetime(10.0f),
		_life(1.0f),
		rotation(0),
		position(0,0,0),
		scale(1.0f,1.0f),
		m_isCreateChild(false)
		{
		}

	SsEffectRenderAtom( SsEffectNode* refdata , SsEffectRenderAtom* _p){
        data = refdata;
		setParent(_p);

		_lifetime = 0;
		position = Vector3(0,0,0);
		scale = Vector2(0,0);
		rotation = 0.0f;
	}

	virtual ~SsEffectRenderAtom(){}

	void	setParent( SsEffectRenderAtom* _p ){ parent = _p; }

	bool	   	isInit(){ return m_isInit; }

	virtual void	Initialize()
	{
		parent = 0;
		m_isInit = false;
		m_isLive = true;
		_lifetime = 10.0f;
		_life = 1.0f;
		rotation = 0;
		position = Vector3(0,0,0);
		scale = Vector2(1,1);
		m_isCreateChild = false;
		m_isInit = false;
	}

    virtual void	update(float delta){}

	virtual void	debugdraw(){}

	Vector3	getPosition() const
	{
		return position;
	}

	void	setPosistion( float x , float y , float z )
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void	setScale( float x , float y )
	{
		scale.x = x;
		scale.y = y;
	}
	void	setRotation( float z  )
	{
		rotation = std::fmod( z , 360 ) ;
	}

	float		getRotation() const { return rotation; }
	Vector2	getScale() const { return scale; }
 	virtual void	count(){}

};




//--------------------------------------------------------------------------
//パーティクル生成能力を持つオブジェクト
//--------------------------------------------------------------------------
class  SsEffectRenderEmitter : public SsEffectRenderAtom
{
public:
	unsigned int myseed;
	//エミッターパラメータ

	CMersenneTwister*	     MT;

	//以前からの移植
	int				maxParticle;    //
	int				delay;
	float				interval;
	float				intervalleft;
	float   		frame;
	float			frameDelta;
    int				burst;


    size_t 			particleCount;


    bool			generate_ok;
	int				drawPriority;



public:
	void	InitParameter()
	{
		if (MT == 0) MT = new CMersenneTwister();
		SsEffectRenderAtom::Initialize();
		delay = 0;
		interval = 0;
		intervalleft = 0;
		frame = 0;
		frameDelta = 0;
		particleCount = 0;
		_exsitTime = 0;

		generate_ok = true;
	}

	SsEffectRenderEmitter() : MT(0){}
	SsEffectRenderEmitter(SsEffectNode* refdata, SsEffectRenderAtom* _p){
		data = refdata;
		parent = _p;
		InitParameter();
	}

	virtual ~SsEffectRenderEmitter()
	{
		if (MT)
		{
			delete MT;
		}
	}
	void			setMySeed( unsigned int seed );
	void			TrushRandom(int loop)
	{
		for ( int i = 0 ; i < loop ; i++ )
			MT->genrand_uint32();
	}

	virtual void	Initialize();

	virtual void	update(float delta);
	virtual void	count(){ particleCount = 0 ; }

};





//--------------------------------------------------------------------------
//パーティクルオブジェクト
//--------------------------------------------------------------------------
class  SsEffectRenderParticle : public SsEffectRenderAtom
{
public:

	SsEffectRenderEmitter*  parentEmitter;
    SsEffectBehavior*		refBehavior;

	Vector2   _baseEmiterPosition;   //もしかしてもう使ってないかも
	Vector2   _position;             //描画用ポジション

	float		_rotation;
	float		_rotationAdd;
	float		_rotationAddDst;
	float		_rotationAddOrg;

	Vector2   _size;
	Vector2  	_startsize;
	Vector2   _divsize;

	SsU8Color	_color;
	SsU8Color	_startcolor;
	SsU8Color	_endcolor;


	float		speed;		//現在持っている速度
	float		firstspeed;
    float		lastspeed;
	Vector2   vector;

	Vector2   _force;
	Vector2   _gravity;

	float 		_radialAccel;
	float 		_tangentialAccel;
	float		direction;
	bool		isTurnDirection;

	Vector2   _execforce;				//処理中の力 最終的には単位当たりの力に変換




public:
	void	InitParameter()
	{

		SsEffectRenderAtom::Initialize();

		_position = Vector2(0,0);
		_baseEmiterPosition = Vector2(0,0);
		_rotation = 0;
		_size = Vector2( 1.0f , 1.0f );
		_startsize = Vector2( 1.0f , 1.0f );
		_divsize = Vector2( 0.0f , 0.0f );
		_force = Vector2(0,0);
		_gravity = Vector2(0,0);
		_radialAccel = 0;
		_tangentialAccel = 0;
		_color = SsU8Color(255,255,255,255) ;
        _startcolor = _color;
        _exsitTime = 0;
		_execforce = Vector2(0,0);
		parentEmitter = 0;
	}



	SsEffectRenderParticle(): parentEmitter(0){}
	SsEffectRenderParticle( SsEffectNode* refdata , SsEffectRenderAtom* _p){
		data = refdata;
		parent = _p;
        InitParameter();
	}

	virtual ~SsEffectRenderParticle(){}
    
	//生成フェーズ
	virtual void	Initialize();

    virtual void	update(float delta);

	virtual void	count()
	{
		if ( parentEmitter )
			parentEmitter->particleCount++;
	}
	virtual void	updateDelta(float delta);
	virtual void 	updateForce(float delta);
};



};
#endif