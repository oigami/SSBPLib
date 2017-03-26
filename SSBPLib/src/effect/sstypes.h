#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include "math/Vector2.h"
#include "player/Util.h"

namespace ss
{


/// テクスチャラップモード
enum class SsTexWrapMode{
	invalid = -1,	/// なし
	clamp,			/// クランプする
	repeat,			/// リピート
	mirror,			/// ミラー
//		num
};


/// テクスチャフィルターモード 画素補間方法
enum class SsTexFilterMode{
	invalid = -1,
	nearlest,	///< ニアレストネイバー
	linear,		///< リニア、バイリニア
//	num
};



//エフェクト関連の定義
//エフェクトのノードタイプ
enum class SsEffectNodeType{
	invalid=-1,
	root,
	emmiter,
	particle,
//	num
};



//エフェクト関連の定義
//エフェクトのブレンドタイプ
enum class SsRenderBlendType{
	invalid=-1,
	Mix,
	Add,
//	num
};



} //namespace ss

