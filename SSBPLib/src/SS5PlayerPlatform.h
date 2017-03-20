/**
*  SS5PlayerPlatform.h
*/

#ifndef SS5PlayerPlatform_h
#define SS5PlayerPlatform_h
#include "SS5Player.h"
#include "effect/sstypes.h"
#include "player/PlayerDef.h"
#include <stdio.h>
#include <string>

namespace ss
{
	struct State;
	struct UserData;
	class Player;

#if 0
	extern TextureID SSTextureLoad(const char* pszFileName, SsTexWrapMode wrapmode, SsTexFilterMode filtermode);
	extern bool SSTextureRelese(TextureID handle);
	extern bool isAbsolutePath(const std::string& strPath);
#endif
	extern void SSDrawSprite(State state, BlendType blendType, BlendType colorBlendType);
#if 0
	extern bool SSGetTextureSize(TextureID handle, int &w, int &h);
#endif
	
#if 0
	//userDataは一時オブジェクトなのでコピーして使ってください
	extern void SSonUserData(Player *player, const UserData &userData, int frame);
	extern int  SSLimitFrame(int frame, int maxFrame);
#endif


};	// namespace ss

#endif
