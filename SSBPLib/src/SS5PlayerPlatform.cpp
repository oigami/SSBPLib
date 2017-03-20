// 
//  SS5Platform.cpp
//
#include "SS5PlayerPlatform.h"
#include "player/Util.h"
#include "player/PlayerDef.h"
#include "math/Matrix.h"

/**
* 各プラットフォームに合わせて処理を作成してください
* DXライブラリ用に作成されています。
*/
#include "DxLib.h"

namespace ss
{

#if 0
	/**
	* テクスチャの読み込み
	*/
	TextureID SSTextureLoad(const char* pszFileName, SsTexWrapMode wrapmode, SsTexFilterMode filtermode)
	{
		/**
		* テクスチャ管理用のユニークな値を返してください。
		* テクスチャの管理はゲーム側で行う形になります。
		* テクスチャにアクセスするハンドルや、テクスチャを割り当てたバッファ番号等になります。
		*
		* プレイヤーはここで返した値とパーツのステータスを引数に描画を行います。
		*/
		TextureID rc = (long)LoadGraph(pszFileName);
		//SpriteStudioで設定されたテクスチャ設定を反映させるための分岐です。
		switch (wrapmode)
		{
		case SsTexWrapMode::clamp:	//クランプ
			break;
		case SsTexWrapMode::repeat:	//リピート
			break;
		case SsTexWrapMode::mirror:	//ミラー
			break;
		}
		switch (filtermode)
		{
		case SsTexFilterMode::nearlest:	//ニアレストネイバー
			break;
		case SsTexFilterMode::linear:	//リニア、バイリニア
			break;
		}

		return rc;
	}
	
	/**
	* テクスチャの解放
	*/
	bool SSTextureRelese(TextureID handle)
	{
		/// 解放後も同じ番号で何度も解放処理が呼ばれるので、例外が出ないように作成してください。
		bool rc = true;

		if ( DeleteGraph((int)handle) == -1 )
		{
			rc = false;
		}

		return rc ;
	}

	/**
	* テクスチャのサイズを取得
	* テクスチャのUVを設定するのに使用します。
	*/
	bool SSGetTextureSize(TextureID handle, int &w, int &h)
	{
		if (GetGraphSize(handle, &w, &h) == -1)
		{
			return false;
		}
		return true;
	}
#endif

	//DXライブラリ用頂点バッファ作成関数
	VERTEX_3D vertex3Dfrom(const ss::SSV3F_C4B_T2F &vct)
	{
		VERTEX_3D v = {
			{ vct.vertices.x, vct.vertices.y, vct.vertices.z },
			vct.colors.b, vct.colors.g, vct.colors.r, vct.colors.a,
			vct.texCoords.u(), vct.texCoords.v()
		};
		return v;
	}
	/**
	* スプライトの表示
	*/
	void SSDrawSprite(State state, BlendType blendType, BlendType colorBlendVertexType, int colorBlendVertexFlags)
	{
		//未対応機能
		//ステータスから情報を取得し、各プラットフォームに合わせて機能を実装してください。
		//X回転、Y回転、カラーブレンド（一部のみ）

		//描画ファンクション
		//
		switch (blendType){
		case BLEND_MIX:		///< 0 ブレンド（ミックス）
			if (state.opacity == 255){
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, state.opacity);
			}
			else{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, state.opacity);
			}
			break;
		case BLEND_MUL:		///< 1 乗算
			SetDrawBlendMode(DX_BLENDMODE_MULA, state.opacity);
			break;
		case BLEND_ADD:		///< 2 加算
			SetDrawBlendMode(DX_BLENDMODE_ADD, state.opacity);
			break;
		case BLEND_SUB:		///< 3 減算
			SetDrawBlendMode(DX_BLENDMODE_SUB, state.opacity);
			break;
		}

		if (colorBlendVertexFlags != 0){
			//RGBのカラーブレンドを設定
			//厳密に再現するには専用のシェーダーを使い、テクスチャにカラー値を合成する必要がある
			//作成する場合はssShader_frag.h、CustomSpriteのコメントとなってるシェーダー処理を参考にしてください。
			if (colorBlendVertexFlags == VERTEX_FLAG_ONE){
				//単色カラーブレンド
			}
			else{
				//頂点カラーブレンド
				//未対応
			}
			switch (colorBlendVertexType){
			case BLEND_MIX:
				break;
			case BLEND_MUL:		///< 1 乗算
				// ブレンド方法は乗算以外未対応
				// とりあえず左上の色を反映させる
				SetDrawBright(state.quad.tl.colors.r, state.quad.tl.colors.g, state.quad.tl.colors.b);
				break;
			case BLEND_ADD:		///< 2 加算
				break;
			case BLEND_SUB:		///< 3 減算
				break;
			}
		}

		/**
		* DXライブラリの3D機能を使用してスプライトを表示します。
		* DXライブラリの3D機能は上方向がプラスになります。
		* 3Dを使用する場合頂点情報を使用して再現すると頂点変形やUV系のアトリビュートを反映させる事ができます。
		*/
		//描画用頂点情報を作成
		SSV3F_C4B_T2F_Quad quad;
		quad = state.quad;

		//原点補正
		float cx = (state.rect.width() * -(state.pivotX - 0.5f));	//デフォルトがpivotX == 0.5になってる
		float cy = (state.rect.height() * +(state.pivotY - 0.5f));

		quad.tl.vertices.x += cx;
		quad.tl.vertices.y += cy;
		quad.tr.vertices.x += cx;
		quad.tr.vertices.y += cy;
		quad.bl.vertices.x += cx;
		quad.bl.vertices.y += cy;
		quad.br.vertices.x += cx;
		quad.br.vertices.y += cy;

		//vertexにworldMatrixをかける
		quad.vertexForeach([&](Vector3& vertex){
			vertex *= state.mat;
		});

		//頂点カラーにアルファを設定
		quad.tl.colors.a = quad.bl.colors.a * state.Calc_opacity / 255;
		quad.tr.colors.a = quad.bl.colors.a * state.Calc_opacity / 255;
		quad.bl.colors.a = quad.bl.colors.a * state.Calc_opacity / 255;
		quad.br.colors.a = quad.bl.colors.a * state.Calc_opacity / 255;

		//DXライブラリ用の頂点バッファを作成する
		VERTEX_3D vertex[4] = {
			vertex3Dfrom(quad.tl),
			vertex3Dfrom(quad.bl),
			vertex3Dfrom(quad.tr),
			vertex3Dfrom(quad.br)
		};
		//3Dプリミティブの表示
		DrawPolygon3DBase(vertex, 4, DX_PRIMTYPE_TRIANGLESTRIP, state.texture.handle, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);	//ブレンドステートを通常へ戻す
	}

#if 0
	/**
	* ユーザーデータの取得
	*/
	void SSonUserData(Player *player, const UserData& userData, int frame)
	{
		//ゲーム側へユーザーデータを設定する関数を呼び出してください。
	}


	/**
	 * フレームに制限をつけるためのイベント
	 * @param frame		これからPlayerが処理したいフレーム番号
	 * @param maxFrame	アニメーションの総フレーム数
	 * @return 制限をかけた後のフレーム番号
	 */
	int SSLimitFrame(int frame, int maxFrame){
		return ss::wrap<int>(frame, 0, maxFrame);		//ループ再生になります
		
		//例:
		//return ss::clamp<int>(frame, 0, maxFrame-1);	//最終フレームで止める
		//return ss::wrap<int>(frame, 3, 7);			//3～6フレームでループさせる
		//if(frame>10){ return 5; }else{ return frame; }//10フレームを過ぎたら5フレームに飛ばす
	}
#endif

#if 0
	/**
	* 文字コード変換
	*/ 
	std::string utf8Togbk(const char *src)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
		unsigned short * wszGBK = new unsigned short[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)src, -1, (LPWSTR)wszGBK, len);

		len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
		char *szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
		std::string strTemp(szGBK);
		if (strTemp.find('?') != std::string::npos)
		{
			strTemp.assign(src);
		}
		delete[]szGBK;
		delete[]wszGBK;
		return strTemp;
	}

	/**
	* windows用パスチェック
	*/ 
	bool isAbsolutePath(const std::string& strPath)
	{
		std::string strPathAscii = utf8Togbk(strPath.c_str());
		if (strPathAscii.length() > 2
			&& ((strPathAscii[0] >= 'a' && strPathAscii[0] <= 'z') || (strPathAscii[0] >= 'A' && strPathAscii[0] <= 'Z'))
			&& strPathAscii[1] == ':')
		{
			return true;
		}
		return false;
	}
#endif
};
