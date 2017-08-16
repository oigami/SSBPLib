#include "CEventListener.hpp"
#include "CPlayer.hpp"
#include <ss/SS5Player.h>
#include <ss/ResultState.h>

namespace SpriteStudio
{
	struct UserData;
}

namespace SpriteStudio
{
	namespace detail
	{
		CPlayer::CPlayer() :m_ss5ResourceManager(std::make_shared<ss::SS5ResourceManager>()), m_ss5Player(nullptr)
		{

		}

		CPlayer::CPlayer(const FilePath & filename) : CPlayer()
		{

			BinaryReader reader(filename);

			if ( !reader.isOpened() )
			{
				return;
			}

			const ByteArray file = reader.readAll();

			if ( file.size() == 0 )
			{
				return;
			}

			m_registerName = filename.narrow();

			m_texturePath = FileSystem::ParentPath(filename).narrow();
			auto res = m_ss5ResourceManager->regist(
				file.data(),		//ssbpデータ
				file.size(),		//ssbpデータサイズ
				m_registerName,	//登録名
				m_texturePath		//テクスチャのあるフォルダを指定
			);

			if ( !res.second )
			{
				return;
			}

			//生成
			m_ss5Player = m_ss5ResourceManager->createPlayer(
				&m_eventListener,					//イベント処理の指定
				m_registerName.c_str()					//登録名
			);

			// アニメーションリストを予め変換しておく

			const auto& animeList = m_ss5Player->getAnimationList();
			m_animationList.reserve(animeList.size());
			for ( auto& i : animeList )
			{
				auto str = CharacterSet::Widen(i);
				m_animationList.push_back(str);
				m_animationNameMaps.insert({ str, i });
			}


			// パーツの名前も予め変換

			m_partsNameList.resize(m_ss5Player->getPartsNum());
			for ( auto& i : step(m_ss5Player->getPartsNum()) )
			{
				const auto& partName = m_ss5Player->getPartName(i);
				m_partsNameList[i] = CharacterSet::Widen(partName);
			}

			// 座標軸が逆なので最初に変換しておく
			setFlip(false);
		}

		CPlayer::~CPlayer()
		{
			release();
		}

		std::shared_ptr<CPlayer> CPlayer::clone()
		{
			auto cplayer = std::make_shared<CPlayer>(*this);
			if ( isEmpty() )
			{
				return cplayer;
			}

			// 必ず存在するので適当に呼び出す
			char x;
			m_ss5ResourceManager->regist(
				&x,		//ssbpデータ
				1,		//ssbpデータサイズ
				m_registerName,	//登録名
				m_texturePath		//テクスチャのあるフォルダを指定
			);

			// Texture は再度読み込むため、一旦クリアする
			cplayer->m_eventListener = CEventListener();

			//生成
			auto ss5Player = m_ss5ResourceManager->createPlayer(
				&cplayer->m_eventListener,					//イベント処理の指定
				m_registerName.c_str()					//登録名
			);
			cplayer->m_ss5Player = ss5Player;

			// 座標軸が逆なので最初に変換しておく
			cplayer->setFlip(false);

			return cplayer;
		}

		bool CPlayer::isEmpty() const
		{
			return m_ss5Player == nullptr;
		}

		bool CPlayer::play(const String & animeName, const int frameNo)
		{
			if ( isEmpty() )
			{
				return false;
			}

			auto it = m_animationNameMaps.find(animeName);
			if ( it != m_animationNameMaps.end() )
			{
				m_playingAnimationName = animeName;
				m_ss5Player->play(it->second, getLoopFrame(frameNo));
				return true;
			}

			return false;
		}

		bool CPlayer::play(const int animationIndex, const int frameNo)
		{
			if ( 0 <= animationIndex && animationIndex < m_animationList.size() )
			{
				return play(m_animationList[animationIndex], frameNo);
			}

			return false;
		}

		const String& CPlayer::playingAnimationName() const
		{
			return m_playingAnimationName;
		}

		const Array<String>& CPlayer::animationNameList() const
		{
			return m_animationList;
		}

		const Array<String>& CPlayer::partsNameList() const
		{
			return m_partsNameList;
		}

		int CPlayer::totalFrame() const
		{
			if ( isEmpty() )
			{
				return 0;
			}

			return m_ss5Player->getMaxFrame();
		}

		int CPlayer::currentFrame() const
		{
			if ( isEmpty() )
			{
				return 0;
			}

			return m_ss5Player->getCurrentFrame();
		}

		void CPlayer::setCurrentFrame(const int frameNo)
		{
			if ( isEmpty() )
			{
				return;
			}

			m_ss5Player->setCurrentFrame(getLoopFrame(frameNo));
		}

		int CPlayer::getLoopFrame(int frameNo) const
		{
			const int total = totalFrame();
			if ( total == 0 )
			{
				return frameNo;
			}

			frameNo %= total;
			if ( frameNo < 0 )
			{
				frameNo += total;
			}
			return frameNo;
		}

		const Array<UserData>& CPlayer::update(const double deltaTime)
		{
			m_eventListener.ResetUserData();

			if ( isEmpty() )
			{
				return m_eventListener.userData();
			}

			m_ss5Player->update(static_cast<float>(deltaTime));
			return m_eventListener.userData();
		}

		void CPlayer::setRotate(const Vec3& rotation)
		{
			if ( isEmpty() )
			{
				return;
			}

			const auto deg = Math::Degrees(Float3(rotation));

			m_ss5Player->setRotation(deg.x, deg.y, deg.z);
		}

		void CPlayer::setScale(const Vec3& scale)
		{
			if ( isEmpty() )
			{
				return;
			}

			const auto f = Float3(scale);
			m_ss5Player->setScale(f.x, f.y, f.z);
		}

		RectF CPlayer::draw(const double x, const double y, const ColorF& diffuse) const
		{
			m_eventListener.ResetLastDrawRect();

			if ( isEmpty() )
			{
				return { 0, 0, 0, 0 };
			}

			m_ss5Player->setPosition(static_cast<float>(x),
									 static_cast<float>(y));

			const auto colorFloat = diffuse.toFloat4();
			m_ss5Player->setColor(colorFloat.x, colorFloat.y, colorFloat.z);
			m_ss5Player->setAlpha(colorFloat.w);

			m_ss5Player->update(0);
			m_ss5Player->draw();

			return m_eventListener.lastDrawRect();
		}

		PartInfo CPlayer::parts(int partIndex) const
		{
			if ( isEmpty() )
			{
				return {};
			}

			if ( !(0 <= partIndex && partIndex < partsNameList().size()) )
			{
				return {};
			}

			PartInfo res{};
			ss::ResultState state;

			m_ss5Player->setPosition(0.0f, 0.0f);
			m_ss5Player->update(0);
			m_ss5Player->getPartState(&state, partIndex);
			const auto center = state.m_worldMatrix->getTranslation();
			res.center = { center.x, center.y };

			res.color = [c = state.getColorLabel()]()
			{
				switch ( c )
				{
				default:
				case ss::ColorLabel::None:		return Palette::White;
				case ss::ColorLabel::Red:		return Palette::Red;
				case ss::ColorLabel::Orange:	return Palette::Orange;
				case ss::ColorLabel::Yellow:	return Palette::Yellow;
				case ss::ColorLabel::Green:		return Palette::Green;
				case ss::ColorLabel::Blue:		return Palette::Blue;
				case ss::ColorLabel::Violet:	return Palette::Violet;
				case ss::ColorLabel::Gray:		return Palette::Gray;
				}
			}();
			res.color.a = state.m_alpha;

			res.boundingRadius = state.boundingRadius;
			res.boundsType = [type = state.m_boundsType]()
			{
				switch ( type )
				{
				default:
				case ss::BoundsType::INVALID:		return BoundsType::Invalid;
				case ss::BoundsType::NONE:			return BoundsType::None;
				case ss::BoundsType::QUAD:			return BoundsType::Quad;
				case ss::BoundsType::AABB:			return BoundsType::AABB;
				case ss::BoundsType::CIRCLE:		return BoundsType::Circle;
				case ss::BoundsType::CIRCLE_SMIN:	return BoundsType::CircleSMin;
				case ss::BoundsType::CIRCLE_SMAX:	return BoundsType::CircleSMax;
				}
			}();

			const auto& q = state.m_quad;
			res.quad.p0 = { q.tl.vertices.x, q.tl.vertices.y };
			res.quad.p1 = { q.bl.vertices.x, q.bl.vertices.y };
			res.quad.p2 = { q.br.vertices.x, q.br.vertices.y };
			res.quad.p3 = { q.tr.vertices.x, q.tr.vertices.y };
			return res;
		}

		void CPlayer::release()
		{
			if ( isEmpty() )
			{
				return;
			}

			m_ss5ResourceManager->destroyPlayer(m_ss5Player);
			m_ss5ResourceManager->unregist(m_registerName);
			m_ss5Player = nullptr;
		}

		void CPlayer::setFlip(const bool isFlip)
		{
			if ( isEmpty() )
			{
				return;
			}

			m_ss5Player->setFlipTB(!isFlip);
		}

		void CPlayer::setMirror(const bool isMirror)
		{
			if ( isEmpty() )
			{
				return;
			}

			m_ss5Player->setFlipLR(isMirror);

		}

		void CPlayer::setPartsVisible(const int partIndex, const bool isVisible)
		{
			if ( isEmpty() )
			{
				return;
			}

			if ( 0 <= partIndex && partIndex < partsNameList().size() )
			{
				m_ss5Player->setPartVisible(partIndex, isVisible);
			}
		}

		bool CPlayer::setFlipPartsVisible(const int partIndex)
		{
			setPartsVisible(partIndex, !partsVisible(partIndex));
			return partsVisible(partIndex);
		}

		bool CPlayer::partsVisible(const int partIndex) const
		{
			if ( isEmpty() )
			{
				return false;
			}

			return m_ss5Player->getPartVisible(partIndex);
		}
	}

}
