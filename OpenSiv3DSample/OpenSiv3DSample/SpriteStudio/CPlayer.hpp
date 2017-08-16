#pragma once
#include <Siv3D/HashMap.hpp>
#include "CEventListener.hpp"
#include "SpriteStudio/Player.hpp"

namespace SpriteStudio
{
	namespace detail
	{
		class CPlayer
		{
		private:

			std::shared_ptr<ss::SS5ResourceManager> m_ss5ResourceManager;	//ssbpファイルの登録先

			ss::SS5Player *m_ss5Player;						//ssbpファイルの再生単位
			CEventListener m_eventListener;					//SS5Player向けのイベント処理の実装

			std::string m_registertName;

			HashMap<String, std::string> m_animationNameMaps;
			Array<String> m_animationList;

			Array<String> m_partsNameList;
			std::string m_texturePath;

		public:

			CPlayer();

			CPlayer(const FilePath& filename);

			~CPlayer();
			std::shared_ptr<CPlayer> clone();

			bool isEmpty() const;

			bool play(const String& animeName, int frameNo = 0);

			bool play(int animationId, int frameNo = 0);

			const Array<String>& animationNameList() const;

			const Array<String>& partsNameList() const;

			const Array<UserData>& update(double deltaTime);

			void setVisible(int partIndex, bool isVisible);

			void setRotate(const Vec3& rotation);

			void setScale(const Vec3& scale);

			void setFlip(bool isFlip);

			void setMirror(bool isMirror);

			RectF draw(double x, double y, ColorF diffuse = Palette::White) const;

			void release();
		};
	}
}
