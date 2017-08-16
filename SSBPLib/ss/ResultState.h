#pragma once
#include <string>
#include <map>
#include "player/PlayerDef.h"
#include "math/Matrix.h"

namespace ss{


/**
 * ResultState
 * ゲーム側に返すパーツステータス。
 * 必要に応じてカスタマイズしてください。
 */
struct ResultState{
	ResultState() :
		m_worldMatrix(nullptr),
		m_alpha(1.0f),
		m_boundsType(BoundsType::NONE),
		boundingRadius(0.0),
		m_colorLabel(nullptr){}


	const Matrix* m_worldMatrix;	/// パーツのワールド行列
	float m_alpha;					/// パーツのアルファ[0:1]
	BoundsType m_boundsType;		/// 当たり判定種類
	float boundingRadius;			/// SS5アトリビュート：当たり半径
	const char* m_colorLabel;		/// カラーラベル
	SSV3F_C4B_T2F_Quad m_quad;		/// 頂点データ、座標、カラー値、UV

	//カラーラベルはstringで埋め込まれているので定数値への変換を用意します
	ColorLabel getColorLabel() const{
		if(m_colorLabel == nullptr){
			return ColorLabel::None;
		}

		static const std::map<std::string, ColorLabel> table = {
			{ ColorLabelString::None,   ColorLabel::None   },
			{ ColorLabelString::Red,    ColorLabel::Red    },
			{ ColorLabelString::Orange, ColorLabel::Orange },
			{ ColorLabelString::Yellow, ColorLabel::Yellow },
			{ ColorLabelString::Green,  ColorLabel::Green  },
			{ ColorLabelString::Blue,   ColorLabel::Blue   },
			{ ColorLabelString::Violet, ColorLabel::Violet },
			{ ColorLabelString::Gray,   ColorLabel::Gray   },
		};
		auto it = table.find(m_colorLabel);
		if(it == table.end()){
			return ColorLabel::None;
		}
		return it->second;
	}
};

} //namespace ss
