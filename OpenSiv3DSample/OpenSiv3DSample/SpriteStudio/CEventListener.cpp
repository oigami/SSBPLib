#pragma once

#include <Siv3D.hpp>
#include <memory>
#include <unordered_map>
#include "ss/SS5ResourceManager.h"
#include "ss/SS5EventListener.h"
#include "CEventListener.hpp"
#include <SpriteStudio/Player.hpp>

namespace ss
{
	class SS5Player;
}

namespace SpriteStudio
{
	namespace detail
	{

		namespace
		{
			Vertex2D Vertex3Dfrom(const ss::SSV3F_C4B_T2F &vct)
			{
				return Vertex2D{
					Float2{ vct.vertices.x, vct.vertices.y },
					Float2{ vct.texCoords.u(), vct.texCoords.v() },
					ColorF(Color(vct.colors.r, vct.colors.g, vct.colors.b, vct.colors.a)).toFloat4()
				};
			}

		}

		CEventListener::CEventListener()
		{
			m_sprite.indices = { 0, 1, 2, 1, 2, 3 };
			m_sprite.vertices.resize(4);
		}

		//テクスチャのロード・リリースのイベント。内部ではPlayer単位で管理されます

		ss::TextureID CEventListener::TextureLoad(const int /*cellMapIndex*/, const std::string & texturePath, const ss::SsTexWrapMode wrapmode, const ss::SsTexFilterMode filtermode)
		{
			const Texture texture(CharacterSet::Widen(texturePath));

			const TextureAddressMode addressMode([wrapmode]()
			{
				switch ( wrapmode )
				{
				default:
				case ss::SsTexWrapMode::invalid:	return TextureAddressMode::Repeat;
				case ss::SsTexWrapMode::clamp:		return TextureAddressMode::Clamp;
				case ss::SsTexWrapMode::repeat:		return TextureAddressMode::Repeat;
				case ss::SsTexWrapMode::mirror:		return TextureAddressMode::Mirror;
				}
			}());

			const TextureFilter filter([filtermode]()
			{
				switch ( filtermode )
				{
				default:
				case ss::SsTexFilterMode::invalid:	return TextureFilter::Linear;
				case ss::SsTexFilterMode::nearlest:	return TextureFilter::Nearest;
				case ss::SsTexFilterMode::linear:	return TextureFilter::Linear;
				}
			}());

			const SamplerState samplerState(addressMode, addressMode, addressMode, filter, filter, filter);

			auto id = texture.id();
			m_textures.insert(std::make_pair(id, TextureWithSamplerState{ texture, samplerState }));

			return id;
		}
		void CEventListener::TextureRelease(const ss::TextureID textureId)
		{
			m_textures.erase(textureId);
		}

		//描画

		void CEventListener::DrawSprite(const ss::SSV3F_C4B_T2F_Quad & quad, const ss::TextureID textureId,
										const ss::BlendType blendType, const ss::BlendType /*colorBlendVertexType*/)
		{
			m_sprite.vertices[0] = Vertex3Dfrom(quad.tl);
			m_sprite.vertices[1] = Vertex3Dfrom(quad.bl);
			m_sprite.vertices[2] = Vertex3Dfrom(quad.tr);
			m_sprite.vertices[3] = Vertex3Dfrom(quad.br);

			auto QuadVertexLoop = [&quad](auto F)
			{
				F(quad.tl.vertices);
				F(quad.bl.vertices);
				F(quad.tr.vertices);
				F(quad.br.vertices);
			};

			QuadVertexLoop([this](const ss::Vector3& v)
			{
				m_lastDrawRect.x = Min(static_cast<double>(v.x), m_lastDrawRect.x);
				m_lastDrawRect.y = Min(static_cast<double>(v.y), m_lastDrawRect.y);
				m_lastDrawRect.w = Max(static_cast<double>(v.x), m_lastDrawRect.w);
				m_lastDrawRect.h = Max(static_cast<double>(v.y), m_lastDrawRect.h);
			});

			//Print
			//	<< m_sprite.vertices[0].pos.asPoint() << m_sprite.vertices[1].pos.asPoint()
			//	<< m_sprite.vertices[2].pos.asPoint() << m_sprite.vertices[3].pos.asPoint();

			RenderStateBlock2D blendState([blendType]() -> BlendState
			{
				switch ( blendType )
				{
				default:
				case ss::BLEND_MIX:	return BlendState::Default;
				case ss::BLEND_MUL:	return BlendState::Multiplicative;
				case ss::BLEND_ADD:	return BlendState::Additive;
				case ss::BLEND_SUB:	return BlendState::Subtractive;
				}
			}());

			// TODO colorBlendVertexType に対応する

			auto texture = m_textures[textureId];

			RenderStateBlock2D samplerState(texture.samplerState);

			m_sprite.draw(texture.tex);
		}

		void CEventListener::ResetUserData()
		{
			m_lastUserData.clear();
		}

		void CEventListener::ResetLastDrawRect() const
		{
			m_lastDrawRect.x = m_lastDrawRect.y = std::numeric_limits<double>::max();
			m_lastDrawRect.w = m_lastDrawRect.h = std::numeric_limits<double>::min();
		}

		RectF CEventListener::lastDrawRect() const
		{
			return {
				m_lastDrawRect.x ,m_lastDrawRect.y,
				m_lastDrawRect.w - m_lastDrawRect.x, m_lastDrawRect.h - m_lastDrawRect.y
			};
		}

		const Array<UserData>& CEventListener::userData() const
		{
			return m_lastUserData;
		}

		void CEventListener::OnUserData(const ss::UserData& ssUserData, int frame)
		{
			UserData userData;
			userData.frameNo = frame;

			userData.partIndex = ssUserData.m_partIndex;

			if ( ssUserData.m_flags & ss::UserData::FLAG_INTEGER )
			{
				userData.integer = ssUserData.m_integer;
			}

			if ( ssUserData.m_flags & ss::UserData::FLAG_POINT )
			{
				userData.point = Point{ ssUserData.m_point[0], ssUserData.m_point[1] };
			}

			if ( ssUserData.m_flags & ss::UserData::FLAG_RECT )
			{

				userData.rect = Rect{
					ssUserData.m_rect[0],ssUserData.m_rect[1],
					ssUserData.m_rect[2],ssUserData.m_rect[3]
				};
			}

			if ( ssUserData.m_flags & ss::UserData::FLAG_STRING )
			{
				userData.str = CharacterSet::Widen(ssUserData.m_str);
			}


			m_lastUserData.push_back(userData);
		}
	}

}
