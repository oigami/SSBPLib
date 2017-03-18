#ifndef __SSPLAYER_EFFECTFUNCTION__
#define __SSPLAYER_EFFECTFUNCTION__

#include "ssplayer_effect2.h"

namespace ss
{

class SsEffectEmitter;
class SsEffectBehavior;

class	SsEffectFunctionExecuter
{
public:

	//新バージョン
	static void	initializeEffect(SsEffectBehavior* beh, SsEffectEmitter* emmiter);
};


};



#endif
