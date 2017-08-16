#pragma once

#include <Siv3D.hpp>

#include "ss/SS5ResourceManager.h"
#include "ss/SS5EventListener.h"

#include <SpriteStudio/Player.hpp>


namespace SpriteStudio
{
	namespace detail
	{


		class CEventListener : public ss::SS5EventListener
		{

		private:

			Sprite m_sprite{ 4, 2 * 3 };

			Array<UserData> m_lastUserData;
			mutable RectF m_lastDrawRect;

			struct TextureWithSamplerState
			{
				Texture tex;
				SamplerState samplerState;
			};

			HashMap<int, TextureWithSamplerState> m_textures;


		public:

			CEventListener();

			//テクスチャのロード・リリースのイベント。内部ではPlayer単位で管理されます
			ss::TextureID TextureLoad(int cellMapIndex, const std::string& texturePath,
									  ss::SsTexWrapMode wrapmode, ss::SsTexFilterMode filtermode) override;

			void TextureRelease(ss::TextureID textureId) override;


			//描画
			void DrawSprite(const ss::SSV3F_C4B_T2F_Quad& quad, ss::TextureID textureId,
							ss::BlendType blendType, ss::BlendType colorBlendVertexType) override;


			void ResetUserData();
			void ResetLastDrawRect() const;
			RectF lastDrawRect() const;

			const Array<UserData>& userData() const;

			void OnUserData(const ss::UserData& userData, int frame) override;
		};
	}
}
