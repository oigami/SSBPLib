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

			m_registerName = CharacterSet::Narrow(filename);

			// TODO [ssbplib] 間違ったファイルを入れたときのエラー処理がない

			m_texturePath = CharacterSet::Narrow(FileSystem::ParentPath(filename));
			m_ss5ResourceManager->regist(
				file.data(),		//ssbpデータ
				file.size(),		//ssbpデータサイズ
				m_registerName,	//登録名
				m_texturePath		//テクスチャのあるフォルダを指定
			);

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

			m_ss5Player->setFlipTB(true);
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

			char x;
			m_ss5ResourceManager->regist(
				&x,		//ssbpデータ
				1,		//ssbpデータサイズ
				m_registerName,	//登録名
				m_texturePath		//テクスチャのあるフォルダを指定
			);

			//生成
			auto ss5Player = m_ss5ResourceManager->createPlayer(
				&cplayer->m_eventListener,					//イベント処理の指定
				m_registerName.c_str()					//登録名
			);
			cplayer->m_ss5Player = ss5Player;
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
				m_ss5Player->play(it->second, frameNo);
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

		const Array<String>& CPlayer::animationNameList() const
		{
			return m_animationList;
		}

		const Array<String>& CPlayer::partsNameList() const
		{
			return m_partsNameList;
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

		RectF CPlayer::draw(const double x, const double y, ColorF diffuse) const
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

		void CPlayer::setFlip(bool isFlip)
		{
			if ( isEmpty() )
			{
				return;
			}

			m_ss5Player->setFlipTB(!isFlip);
		}

		void CPlayer::setMirror(bool isMirror)
		{
			if ( isEmpty() )
			{
				return;
			}

			m_ss5Player->setFlipLR(isMirror);
		}

		void CPlayer::setVisible(int partIndex, bool isVisible)
		{
			if ( isEmpty() )
			{
				return;
			}

			m_ss5Player->setPartVisible(partIndex, isVisible);
		}
	}

}
