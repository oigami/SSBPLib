#pragma once


namespace ss{


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
