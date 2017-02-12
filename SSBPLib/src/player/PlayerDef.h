#pragma once

namespace ss{


/**
 * 再生するフレームに含まれるパーツデータのフラグ
 */
enum {
	PART_FLAG_INVISIBLE = 1 << 0,		/// 非表示
	PART_FLAG_FLIP_H = 1 << 1,		/// 横反転
	PART_FLAG_FLIP_V = 1 << 2,		/// 縦反転

	// optional parameter flags
	PART_FLAG_CELL_INDEX = 1 << 3,		/// セル番号
	PART_FLAG_POSITION_X = 1 << 4,		/// X座標
	PART_FLAG_POSITION_Y = 1 << 5,		/// Y座標
	PART_FLAG_POSITION_Z = 1 << 6,		/// Z座標
	PART_FLAG_PIVOT_X = 1 << 7,		/// 原点オフセットX
	PART_FLAG_PIVOT_Y = 1 << 8,		/// 原点オフセットY
	PART_FLAG_ROTATIONX = 1 << 9,		/// X回転
	PART_FLAG_ROTATIONY = 1 << 10,		/// Y回転
	PART_FLAG_ROTATIONZ = 1 << 11,		/// Z回転
	PART_FLAG_SCALE_X = 1 << 12,		/// スケールX
	PART_FLAG_SCALE_Y = 1 << 13,		/// スケールY
	PART_FLAG_OPACITY = 1 << 14,		/// 不透明度
	PART_FLAG_COLOR_BLEND = 1 << 15,		/// カラーブレンド
	PART_FLAG_VERTEX_TRANSFORM = 1 << 16,		/// 頂点変形

	PART_FLAG_SIZE_X = 1 << 17,		/// サイズX
	PART_FLAG_SIZE_Y = 1 << 18,		/// サイズY

	PART_FLAG_U_MOVE = 1 << 19,		/// UV移動X
	PART_FLAG_V_MOVE = 1 << 20,		/// UV移動Y
	PART_FLAG_UV_ROTATION = 1 << 21,		/// UV回転
	PART_FLAG_U_SCALE = 1 << 22,		/// UVスケールX
	PART_FLAG_V_SCALE = 1 << 23,		/// UVスケールY
	PART_FLAG_BOUNDINGRADIUS = 1 << 24,		/// 当たり半径

	PART_FLAG_INSTANCE_KEYFRAME = 1 << 25,		/// インスタンス
	PART_FLAG_EFFECT_KEYFRAME = 1 << 26,		/// エフェクト

	NUM_PART_FLAGS
};

/**
 * 頂点変形フラグ
 */
enum {
	VERTEX_FLAG_LT = 1 << 0,
	VERTEX_FLAG_RT = 1 << 1,
	VERTEX_FLAG_LB = 1 << 2,
	VERTEX_FLAG_RB = 1 << 3,
	VERTEX_FLAG_ONE = 1 << 4	// color blend only
};

/**
 * インスタンスループ設定フラグ
 */
enum {
	INSTANCE_LOOP_FLAG_INFINITY = 1 << 0,		//
	INSTANCE_LOOP_FLAG_REVERSE = 1 << 1,
	INSTANCE_LOOP_FLAG_PINGPONG = 1 << 2,
	INSTANCE_LOOP_FLAG_INDEPENDENT = 1 << 3,
};

//エフェクトアトリビュートのループフラグ
enum {
	EFFECT_LOOP_FLAG_INDEPENDENT = 1 << 0,
};

/**
 * Animation Part Type
 */
enum
{
	PARTTYPE_INVALID = -1,
	PARTTYPE_NULL,			/// null。領域を持たずSRT情報のみ。ただし円形の当たり判定は設定可能。
	PARTTYPE_NORMAL,		/// 通常パーツ。領域を持つ。画像は無くてもいい。
	PARTTYPE_TEXT,			/// テキスト(予約　未実装）
	PARTTYPE_INSTANCE,		/// インスタンス。他アニメ、パーツへの参照。シーン編集モードの代替になるもの
	PARTTYPE_EFFECT,		// ss5.5対応エフェクトパーツ
	PARTTYPE_NUM
};

/*
 * 当たり判定の種類
 */
enum
{
	INVALID = -1,
	NONE,			///< 当たり判定として使わない。
	QUAD,			///< 自在に変形する四辺形。頂点変形など適用後の４角を結んだ領域。最も重い。
	AABB,			///< 回転しない全体を囲む矩形で交差判定
	CIRCLE,			///< 真円の半径で距離により判定する
	CIRCLE_SMIN,	///< 真円の半径で距離により判定する (スケールはx,yの最小値をとる）
	CIRCLE_SMAX,	///< 真円の半径で距離により判定する (スケールはx,yの最大値をとる）
	num
};

/**
 * αブレンド方法
 */
enum BlendType
{
	BLEND_MIX,		///< 0 ブレンド（ミックス）
	BLEND_MUL,		///< 1 乗算
	BLEND_ADD,		///< 2 加算
	BLEND_SUB		///< 3 減算
};

/*
Common\Loader\sstypes.hに実際の定義があります。
/// テクスチャラップモード
namespace SsTexWrapMode
{
	enum _enum
	{
		invalid = -1,	/// なし
		clamp,			/// クランプする
		repeat,			/// リピート
		mirror,			/// ミラー
		num
	};
};

/// テクスチャフィルターモード 画素補間方法
namespace SsTexFilterMode
{
	enum _enum
	{
		invalid = -1,
		nearlest,	///< ニアレストネイバー
		linear,		///< リニア、バイリニア
		num
	};
};
*/

//カラーラベル定数
#define COLORLABELSTR_NONE		""
#define COLORLABELSTR_RED		"Red"
#define COLORLABELSTR_ORANGE	"Orange"
#define COLORLABELSTR_YELLOW	"Yellow"
#define COLORLABELSTR_GREEN		"Green"
#define COLORLABELSTR_BLUE		"Blue"
#define COLORLABELSTR_VIOLET	"Violet"
#define COLORLABELSTR_GRAY		"Gray"
enum
{
	COLORLABEL_NONE,		///< 0 なし
	COLORLABEL_RED,			///< 1 赤
	COLORLABEL_ORANGE,		///< 2 オレンジ
	COLORLABEL_YELLOW,		///< 3 黄色
	COLORLABEL_GREEN,		///< 4 緑
	COLORLABEL_BLUE,		///< 5 青
	COLORLABEL_VIOLET,		///< 6 紫
	COLORLABEL_GRAY,		///< 7 灰色
};

} //namespace ss
