#include "ssplayer_effect2.h"
#include <stdio.h>
#include <cstdlib>
#include "player/CustomSprite.h"
#include "player/PlayerDef.h"
#include "math/Matrix.h"
#include "SS5EventListener.h"


namespace ss{

double OutQuad(double t, double totaltime, double max, double min)
{
	if (totaltime == 0.0) return 0.0;

	if (t > totaltime) t = totaltime;
	max -= min;
	t /= totaltime;
	return -max*t*(t - 2) + min;
}

//現在時間から産出される位置を求める
//time変数から求められる式とする
//パーティクル座標計算のコア
void	SsEffectEmitter::updateParticle(float time, particleDrawData* p, bool recalc )
{
	float _t = (float)(time - p->stime);
	float _tm = (float)(_t - 1.0f );
	float _t2 = _t * _t; //(経過時間の二乗)
	float _life = (float)( p->lifetime - p->stime);

	if ( _life == 0 ) return ;
	float _lifeper = (float)( _t / _life );


	//_t = 0時点の値を作る
	//シード値で固定化されることが前提
	unsigned long pseed = seedList[p->id % seedTableLen];


	//自身のシード値、エミッターのシード値、親パーティクルのＩＤをシード値とする
	rand.init_genrand((pseed + emitterSeed + p->pid + seedOffset));

	float rad = particle.angle + (rand.genrand_float32() * (particle.angleVariance ) - particle.angleVariance/2.0f);
	//float speed = rand.genrand_float32() * particle.speed;
	float speed = particle.speed + ( particle.speed2 * rand.genrand_float32() );



	//接線加速度
	float addr = 0;
	if ( particle.useTanAccel )
	{
		float accel = particle.tangentialAccel + (rand.genrand_float32() * particle.tangentialAccel2);

		float _speed = speed;
		if ( _speed <= 0 )_speed = 0.1f;
		//平均角速度を求める
		float l = _life * _speed * 0.2f; //円の半径
		float c = 3.14 * l;

		//最円周 / 加速度(pixel)
		addr = ( accel / c ) * _t;
	}

	float x = cos(rad + addr) * speed * (float)_t;
	float y = sin(rad + addr) * speed * (float)_t;

	if ( particle.useTransSpeed )
	{
		float transspeed = particle.transSpeed + ( particle.transSpeed2 * rand.genrand_float32() );
        float speedadd = transspeed / _life;

		float addtx =  cos(rad + addr) * speed;
		float addtx_trans =  cos(rad + addr) * speedadd;

		float addx = (( addtx_trans * _t ) + addtx ) * (_t+1.0f) / 2.0f;


		float addty =  sin(rad + addr) * speed;
		float addty_trans =  sin(rad + addr) * speedadd;

		float addy = (( addty_trans * _t ) + addty ) * ( _t+1.0f) / 2.0f;

		x = addx;
		y = addy;

	}


	//重力加速度の計算
	if ( particle.useGravity )
	{
		x += (0.5 * particle.gravity.x * (_t2));
		y += (0.5 * particle.gravity.y * (_t2));
	}

	//初期位置オフセット
	float ox,oy;
	ox = oy = 0;
	if ( particle.useOffset )
	{
		ox = (particle.offset.x + (particle.offset2.x * rand.genrand_float32()));
		oy = (particle.offset.y + (particle.offset2.y * rand.genrand_float32()));
	}

	//角度初期値
	p->rot = 0;
	if ( particle.useRotation )
	{
		p->rot = particle.rotation + (rand.genrand_float32() * particle.rotation2);
		float add = particle.rotationAdd + (rand.genrand_float32() * particle.rotationAdd2);

		//角度変化
		if ( particle.useRotationTrans )
		{
			//到達までの絶対時間
			float lastt = _life * particle.endLifeTimePer;

			float addf = 0;
			if ( lastt == 0 )
			{
			  	float addrf =  (add * particle.rotationFactor) * _t;
				p->rot+=addrf;
			}else{
				//1フレームで加算される量
				addf = (add * particle.rotationFactor - add) / lastt;

				//あまり時間
				float mod_t = _t - lastt;
				if ( mod_t < 0 ) mod_t = 0;

				//現在時間（最終時間でリミット
				float nowt = _t;
				if ( nowt > lastt ) nowt = lastt;

				//最終項 + 初項 x F / 2
				float final_soul = add + addf * nowt;
				float addrf = (final_soul + add) * (nowt + 1.0f) / 2.0f;
				addrf -= add;
				addrf += (mod_t * (final_soul)); //あまりと終項の積を加算
				p->rot+=addrf;
			}
		}else{
			p->rot+= ( (add*_t) );
		}
	}

	//カラーの初期値、カラーの変化
	p->color.a = 0xff;
	p->color.r = 0xff;
	p->color.g = 0xff;
	p->color.b = 0xff;

	if ( particle.useColor)
	{
		p->color.a = particle.initColor.a + (rand.genrand_float32() * particle.initColor2.a );
		p->color.r = particle.initColor.r + (rand.genrand_float32() * particle.initColor2.r );
		p->color.g = particle.initColor.g + (rand.genrand_float32() * particle.initColor2.g );
		p->color.b = particle.initColor.b + (rand.genrand_float32() * particle.initColor2.b );
	}

	if ( particle.useTransColor )
	{
		SSColor4B ecolor;
		ecolor.a = particle.transColor.a + (rand.genrand_float32() * particle.transColor2.a );
		ecolor.r = particle.transColor.r + (rand.genrand_float32() * particle.transColor2.r );
		ecolor.g = particle.transColor.g + (rand.genrand_float32() * particle.transColor2.g );
		ecolor.b = particle.transColor.b + (rand.genrand_float32() * particle.transColor2.b );

		p->color.a = lerp<unsigned char>( p->color.a , ecolor.a , _lifeper );
		p->color.r = lerp<unsigned char>( p->color.r , ecolor.r , _lifeper );
		p->color.g = lerp<unsigned char>( p->color.g , ecolor.g , _lifeper );
		p->color.b = lerp<unsigned char>( p->color.b , ecolor.b , _lifeper );
	}

	if ( particle.useAlphaFade )
	{

		float start = particle.alphaFade;
		float end = particle.alphaFade2;
        float per = _lifeper * 100.0f;


		if ( per < start )
		{
			float alpha = (start - per) / start;
			p->color.a*= 1.0f - alpha;
		}else{

			if ( per > end )
			{

				if (end>=100.0f)
				{
					p->color.a = 0;
				}else{
					float alpha = (per-end) / (100.0f-end);
                    if ( alpha >=1.0f ) alpha = 1.0f;

					p->color.a*= 1.0f - alpha;
				}
			}
		}
	}


	//スケーリング
	p->scale.x = 1.0f;
	p->scale.y = 1.0f;
	float scalefactor = 1.0f;

	if ( particle.useInitScale )
	{
		p->scale.x = particle.scale.x + (rand.genrand_float32() * particle.scaleRange.x );
		p->scale.y = particle.scale.y + (rand.genrand_float32() * particle.scaleRange.y );

        scalefactor = particle.scaleFactor + (rand.genrand_float32() * particle.scaleFactor2 );


	}

	if ( particle.useTransScale )
	{
		Vector2 s2;
		float sf2;
		s2.x = particle.transscale.x + (rand.genrand_float32() * particle.transscaleRange.x );
		s2.y = particle.transscale.y + (rand.genrand_float32() * particle.transscaleRange.y );

		sf2 = particle.transscaleFactor + (rand.genrand_float32() * particle.transscaleFactor2 );

		p->scale.x = lerp<float>( p->scale.x , s2.x , _lifeper );
		p->scale.y = lerp<float>( p->scale.y , s2.y , _lifeper );
        scalefactor = lerp<float>( scalefactor , sf2 , _lifeper );

	}

	p->scale.x*=scalefactor;
	p->scale.y*=scalefactor;

	p->x = x + ox + position.x;//エミッタからのオフセットを加算
	p->y = y + oy + position.y;//エミッタからのオフセットを加算


  	//指定の点へよせる
	if ( particle.usePGravity )
	{

		//生成地点からの距離
		Vector2 v = Vector2(  particle.gravityPos.x - (ox + position.x) ,
                         particle.gravityPos.y - (oy + position.y) );


		Vector2 nv = v;
		nv.normalize();

		float gp = particle.gravityPower;
		if (gp > 0) {
			Vector2 v2 = Vector2(p->x, p->y);
			float len = v.length(); // 生成位置からの距離
			float et = (len / gp)*0.90f;;

			float _gt = _t;
			if ( _gt >= (int)et )
			{
				_gt = et*0.90f;// + (_t / _life *0.1f);
			}

			nv = nv * gp * _gt;
			p->x += nv.x;
			p->y += nv.y;


			float blend = OutQuad(_gt, et, 0.9f, 0.0f);
			blend = blend; // *gp;
			blend += (_t / _life *0.1f);

			p->x = lerp<float>(p->x, particle.gravityPos.x, blend);
			p->y = lerp<float>(p->y, particle.gravityPos.y, blend);

		}
		else {
			nv = nv * gp * _t;
			// パワーマイナスの場合は単純に反発させる
			// 距離による減衰はない
			p->x += nv.x;
			p->y += nv.y;
		}
#if 0
		float gx = OutQuad(_t *0.8f, _life, particle.gravityPos.x, ox + position.x);
		float gy = OutQuad(_t *0.8f, _life, particle.gravityPos.y, oy + position.y);

		float gp = particle.gravityPower;
		if (gp < 0)
		{
			p->x = lerp<float>(p->x, -gx, -gp);
			p->y = lerp<float>(p->y, -gy, -gp);

		}
		else
		{
			p->x = lerp<float>(p->x, gx, gp);
			p->y = lerp<float>(p->y, gy, gp);
		}
#endif
	}

    //前のフレームからの方向を取る
	p->direc = 0.0f;
	if ( particle.useTurnDirec && recalc==false )
	{
		particleDrawData dp;
        dp = *p;

//		if ( time > 0.0f )
		{
			updateParticle(time + 1.0f, &dp, true);
			p->direc = Vec2Util::getAngle360(Vector2(1, 0), Vector2(p->x - dp.x, p->y - dp.y));
			p->direc += SSDegToRad(90) + SSDegToRad(particle.direcRotAdd);
		}
	}


}


bool compare_life( emitPattern& left,  emitPattern& right)
{
	if (left.life == right.life)
	{
		if (left.uid < right.uid) return true;
	}
	return left.life < right.life ;
}

void	SsEffectEmitter::precalculate2()
{

	rand.init_genrand( emitterSeed );

	_emitpattern.clear();
	//_lifeExtend.clear();
	_offsetPattern.clear();

	if ( particleExistList == 0 )
	{
		particleExistList = new particleExistSt[emitter.emitmax]; //存在しているパーティクルが入る計算用バッファ
	}

	memset( particleExistList , 0 , sizeof(particleExistSt) * emitter.emitmax );

	if ( emitter.emitnum < 1 ) emitter.emitnum = 1;

	int cycle =  (int)(( (float)(emitter.emitmax *emitter.interval)  / (float)emitter.emitnum ) + 0.5f) ;
    int group =  emitter.emitmax / emitter.emitnum;

	int extendsize = emitter.emitmax*LIFE_EXTEND_SCALE;
	if (extendsize < LIFE_EXTEND_MIN) extendsize = LIFE_EXTEND_MIN;

	int shot = 0;
	int offset = particle.delay;
	for (int i = 0; i < emitter.emitmax; i++)
	{
		if (shot >= emitter.emitnum)
		{
			shot = 0;
			offset += emitter.interval;
		}
		_offsetPattern.push_back(offset);
		shot++;
	}

	for (int i = 0; i < extendsize; i++)
	{
		emitPattern e;
		e.uid = i;
		e.life = emitter.particleLife + emitter.particleLife2 * rand.genrand_float32();
		e.cycle = cycle;
		if (e.life > cycle)
		{
			e.cycle = e.life;
		}

		_emitpattern.push_back(e);
	}

	particleListBufferSize = emitter.emitmax;

	rand.init_genrand((emitterSeed));

	seedTableLen = particleListBufferSize * 3;
	seedList.resize(seedTableLen);

	//各パーティクルＩＤから参照するシード値をテーブルとして作成する
	for (size_t i = 0; i < seedTableLen; i++)
	{
    	seedList[i] = rand.genrand_uint32();
	}


}





void SsEffectEmitter::updateEmitter(double _time, int slide)
{
	int onum = _offsetPattern.size();
	int pnum = _emitpattern.size();
	slide = slide * SEED_MAGIC;


	for (int i = 0; i < onum; i++)
	{
		int slide_num = (i + slide) % pnum;

		emitPattern* targetEP = &_emitpattern[slide_num];

		int t = (int)(_time - _offsetPattern[i]);

		particleExistList[i].exist = false;
		particleExistList[i].born = false;

		if (targetEP->cycle != 0)
		{
			int loopnum = t / targetEP->cycle;
			int cycle_top = loopnum * targetEP->cycle;

			particleExistList[i].cycle = loopnum;

			particleExistList[i].stime = cycle_top + _offsetPattern[i];
			particleExistList[i].endtime = particleExistList[i].stime + targetEP->life;// + _lifeExtend[slide_num];

			if ((double)particleExistList[i].stime <= _time && (double)particleExistList[i].endtime > _time)
			{
				particleExistList[i].exist = true;
				particleExistList[i].born = true;
			}

			if (!this->emitter.Infinite)
			{
				if (particleExistList[i].stime >= this->emitter.life) //エミッターが終了している
				{
					particleExistList[i].exist = false;    //作られてない

														   //最終的な値に計算し直し <-事前計算しておくといいかも・
					int t = this->emitter.life - _offsetPattern[i];
					int loopnum = t / targetEP->cycle;

					int cycle_top = loopnum * targetEP->cycle;

					particleExistList[i].stime = cycle_top + _offsetPattern[i];

					particleExistList[i].endtime = particleExistList[i].stime + targetEP->life;// + _lifeExtend[slide_num];
					particleExistList[i].born = false;
				}
				else 
				{
					particleExistList[i].born = true;
				}
			}

			if (t < 0) {
				particleExistList[i].exist = false;
				particleExistList[i].born = false;
			}
		}
	}

}



const particleExistSt*	SsEffectEmitter::getParticleDataFromID(int id) const
{
	return &particleExistList[id];
}


} //namespace ss
