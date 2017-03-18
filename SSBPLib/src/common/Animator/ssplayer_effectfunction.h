#ifndef __SSPLAYER_EFFECTFUNCTION__
#define __SSPLAYER_EFFECTFUNCTION__

#include "ssplayer_effect.h"
#include "ssplayer_effect2.h"

namespace ss
{

class SsEffectEmitter;

class	SsEffectFunctionExecuter
{
public:
#if 0
	static void	initalize( SsEffectBehavior* beh , SsEffectRenderEmitter* emmiter);
	static void	updateEmmiter( SsEffectBehavior* beh , SsEffectRenderEmitter* emmiter);
	static void	initializeParticle( SsEffectBehavior* beh , SsEffectRenderEmitter* e , SsEffectRenderParticle* particle );
	static void	updateParticle( SsEffectBehavior* beh , SsEffectRenderEmitter* e , SsEffectRenderParticle* particle );
#endif
	//新バージョン
	static void	initializeEffect(SsEffectBehavior* beh, SsEffectEmitter* emmiter);
};


};



#endif
