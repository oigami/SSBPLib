#include <stdio.h>
#include <cstdlib>
#include <algorithm>

#include "ssplayer_effect.h"
#include "ssplayer_effectfunction.h"

#include "../../SS5PlayerPlatform.h"
#include "player/PlayerDef.h"
#include "math/Matrix.h"

namespace ss
{

class SsEffectRenderParticle;


//SSの乱数固定ボタンがビットに対応
static  int seed_table[] =
{
	485,   //0
	583,   //1
	814,   //2
	907,   //3
	1311,  //4
	1901,  //5
	2236,  //6
	3051,  //7
	3676,  //8
	4338,  //9
	4671,  //10
	4775,  //11
	4928,  //12
	4960,  //13
	5228,  //14
	5591,  //15
	5755,  //16
	5825,  //17
	5885,  //18
	5967,  //19
	6014,  //20
	6056,  //21
	6399,  //22
	6938,  //23
	7553,  //24
	8280,  //25
	8510,  //26
	8641,  //27
	8893,  //28
	9043,  //29
	31043, //30
	31043  //31
};


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void	SsEffectRenderEmitter::setMySeed( unsigned int seed )
{

	if (seed > 31){
		this->MT->init_genrand(seed);

	}
	else{
		this->MT->init_genrand(seed_table[seed]);
	}
	myseed = seed;
}


//----------------------------------------------------------------------
//生成フェーズ
//----------------------------------------------------------------------
void	SsEffectRenderEmitter::Initialize()
{
	if ( !m_isInit ){		//子要素を解析(一度だけ）

		if (this->data->GetMyBehavior()){
			SsEffectFunctionExecuter::initalize( this->data->GetMyBehavior() , this );
		}
        intervalleft = this->interval;
	}


	m_isInit = true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void	SsEffectRenderEmitter::update(float delta)
{

	_exsitTime+=delta;
	_life = _lifetime - _exsitTime;
	intervalleft+=delta;

	if ( this->parent )
	{
		//以下は仮
		this->position = this->parent->position;
		this->rotation = this->parent->rotation;
		this->scale = this->parent->scale;
		this->alpha = this->parent->alpha;
	}
	if (this->data->GetMyBehavior())
	{
		SsEffectFunctionExecuter::updateEmmiter( this->data->GetMyBehavior() , this );
	}
}


//----------------------------------------------------------------------
//パーティクルクラス
//----------------------------------------------------------------------
//生成フェーズ
void	SsEffectRenderParticle::Initialize()
{


	if ( !m_isInit )
	{
		//子要素を解析  基本的にエミッターのみの生成のはず　（Ｐではエラーでいい）
		//処理を省いてエミッター生成のつもりで作成する
		//パーティクルに紐づいたエミッターが生成される
		parentEmitter = 0;

		parentEmitter = static_cast<SsEffectRenderEmitter*>(this->parent);

		if ( parentEmitter->data == 0 )
		{
			this->_life = 0.0f;
			m_isInit = false;
			return ;
		}

		this->refBehavior = parentEmitter->data->GetMyBehavior();
		if ( refBehavior )
		{
			 SsEffectFunctionExecuter::initializeParticle( refBehavior , parentEmitter , this );

		}
	}

	m_isInit = true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void	SsEffectRenderParticle::update(float delta)
{


	 //_rotation = 0;
     if ( !this->isInit() )return ;
	 this->position.x = this->_position.x;
	 this->position.y = this->_position.y;
	 this->scale = this->parent->scale;
	 this->alpha = this->parent->alpha;

	 //初期値突っ込んでおく、パーティクルメソッドのアップデートに持ってく？
	 this->_color = this->_startcolor;

	//this->parent
	if ( parentEmitter )
	{
    	updateDelta( delta );

		if ( refBehavior )
		{
			 SsEffectFunctionExecuter::updateParticle( refBehavior , parentEmitter , this );
		}

		updateForce( delta );

		if (parent->_life <= 0.0f)
		{
		}else{
			//仮
			this->position.x = this->_position.x;
			this->position.y = this->_position.y;
		}

	}

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void	SsEffectRenderParticle::updateDelta(float delta)
{
	_rotation += (_rotationAdd*delta);

	_exsitTime+=delta;
	_life = _lifetime - _exsitTime;

	Vector2	tangential = Vector2( 0 , 0 );

	//接線加速度の計算
	Vector2  radial = Vector2(this->_position.x,this->_position.y);
	radial.normalize();
	
	tangential = radial;

    radial = radial * _radialAccel;

	float newY = tangential.x;
	tangential.x = -tangential.y;
	tangential.y = newY;

	tangential = tangential* _tangentialAccel;

	Vector2 tmp = radial + tangential;

	this->_execforce = tmp;


}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void 	SsEffectRenderParticle::updateForce(float delta)
{
	this->_force = _gravity;
	Vector2 ff = (this->vector * this->speed) + this->_execforce + this->_force;


	if ( isTurnDirection ){
		this->direction = Vec2Util::getAngle360( Vector2( 1.0f , 0.0f ) , ff ) - (float)SSDegToRad(90);
	}
	else{
        this->direction = 0;
	}

	//フォースを加算
	this->_position.x+= (ff.x * delta );
	this->_position.y+= (ff.y * delta );

}

};


