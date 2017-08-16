#include "CEventListener.hpp"
#include "CPlayer.hpp"
#include <ss/SS5Player.h>

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

		void CPlayer::setVisible(const int partIndex, const bool isVisible)
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
	}

}
