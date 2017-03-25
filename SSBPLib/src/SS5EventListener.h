#pragma once
#include "SS5PlayerTypes.h"

namespace ss{
class Player;
struct UserData;

//親パーツ情報 
struct ParentPartState{	//todo:そのうちSRTMatrixを渡すようにしたい
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;
	SSColor4B m_color;
};


/** ロードイベントなどを捕まえるのでこれを継承して作ってください */
class SS5EventListener{
public:
	SS5EventListener(){}
	virtual ~SS5EventListener(){}


	//テクスチャのロード・リリースのイベント。内部ではPlayer単位で管理されます
	virtual TextureID SSTextureLoad(const char* pszFileName, SsTexWrapMode wrapmode, SsTexFilterMode filtermode) = 0;
	virtual void SSTextureRelease(TextureID handle) = 0;

	//テクスチャサイズの取得
	virtual void SSGetTextureSize(TextureID handle, int* width, int* height) = 0;

	//描画
	virtual void SSDrawSprite(const SSV3F_C4B_T2F_Quad& quad, TextureID textureId, BlendType blendType, BlendType colorBlendVertexType, int colorBlendVertexFlags) = 0;


	/**
	 * インスタンスアニメーションのロード・リリースのイベント。ロード成功時はtrue返してください
	 * @param parentPartIndex	親になるパーツのindex
	 * @param parentPartName	親になるパーツの名前
	 * @param animName			再生アニメーション名
	 */
	virtual bool ChildPlayerLoad(int parentPartIndex, const std::string& parentPartName, const std::string& animName) = 0;
	virtual void ChildPlayerRelease(int parentPartIndex, const std::string& parentPartName) = 0;

	/**
	 * 更新時などに呼び出されるSet系のイベント。
	 * 親のパーツの情報を伝播させるために必要になります。
	 * Player内部ではChildPlayerの制御はしないため、このイベントを活用してください。
	 * @param parentPartIndex	親パーツのindex
	 * @param parentPartName	親パーツの名前
	 * @param parentpartState	親パーツの情報(座標とかが入っているので、子供のPlayerにセットしてください)
	 * @param frame				インスタンスアニメの設定で指定したframeの計算結果
	 * @param independent		インスタンスアニメの設定で独立動作を指定していればtrue
	 */
	virtual void ChildPlayerSetFrame(
		int parentPartIndex, const std::string& parentPartName,
		const ParentPartState& parentPartState, int frame, bool independent
	) = 0;

	/** 描画イベント */
	virtual void ChildPlayerDraw(int parentPartIndex, const std::string& parentPartName) = 0;

	

	/**
	 * ユーザーデータがあったときに呼ばれる
	 * @param player
	 * @param userData	一時オブジェクトなのでコピーして使ってください
	 * @param frame		userDataが設定されているフレーム
	 */
	virtual void onUserData(Player* player, const UserData& userData, int frameNo) = 0;

	/**
	 * 再生フレームに制限をかけます
	 * @param frame		これからPlayerが処理したいフレーム番号
	 * @param maxFrame	アニメーションの総フレーム数
	 * @return 制限をかけた後のフレーム番号
	 */
	virtual int limitFrame(Player* player, int frame, int maxFrame){
		return ss::wrap<int>(frame, 0, maxFrame);		//ループ再生になります
		
		//例:
		//return ss::clamp<int>(frame, 0, maxFrame-1);	//最終フレームで止める
		//return ss::wrap<int>(frame, 3, 7);			//3～6フレームでループさせる
		//if(frame>10){ return 5; }else{ return frame; }//10フレームを過ぎたら5フレームに飛ばす
	}
	
};


} //namespace ss

