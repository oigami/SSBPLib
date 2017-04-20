#pragma once

namespace ss{

/**
 * ResluteState
 * ゲーム側に返すパーツステータス。
 * 必要に応じてカスタマイズしてください。
 */
struct ResluteState
{
	int flags;						/// このフレームで更新が行われるステータスのフラグ
	int cellIndex;					/// パーツに割り当てられたセルの番号
	float x;						/// SS5アトリビュート：X座標
	float y;						/// SS5アトリビュート：Y座標
	float z;						/// SS5アトリビュート：Z座標
	float pivotX;					/// 原点Xオフセット＋セルに設定された原点オフセットX
	float pivotY;					/// 原点Yオフセット＋セルに設定された原点オフセットY
	float rotationX;				/// X回転
	float rotationY;				/// Y回転
	float rotationZ;				/// Z回転
	float scaleX;					/// Xスケール
	float scaleY;					/// Yスケール
	int opacity;					/// 不透明度（0～255）
	float size_X;					/// SS5アトリビュート：Xサイズ
	float size_Y;					/// SS5アトリビュート：Xサイズ
	float uv_move_X;				/// SS5アトリビュート：UV X移動
	float uv_move_Y;				/// SS5アトリビュート：UV Y移動
	float uv_rotation;				/// SS5アトリビュート：UV 回転
	float uv_scale_X;				/// SS5アトリビュート：UV Xスケール
	float uv_scale_Y;				/// SS5アトリビュート：UV Yスケール
	float boundingRadius;			/// SS5アトリビュート：当たり半径
	BlendType colorBlendVertexFunc;	/// SS5アトリビュート：カラーブレンドのブレンド方法
	bool flipX;						/// 横反転
	bool flipY;						/// 縦反転
	bool isVisibled;				/// 非表示

	int	part_type;					/// パーツ種別
	int	part_boundsType;			/// 当たり判定種類
	int	part_alphaBlendType;		/// BlendType
	int	part_labelcolor;			/// ラベルカラー
};

} //namespace ss
