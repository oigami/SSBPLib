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

			std::string m_registerName;

			HashMap<String, std::string> m_animationNameMaps;
			Array<String> m_animationList;

			Array<String> m_partsNameList;
			std::string m_texturePath;

			String m_playingAnimationName;

		public:

			CPlayer();

			CPlayer(const FilePath& filename);

			~CPlayer();
			std::shared_ptr<CPlayer> clone();

			bool isEmpty() const;

			bool play(const String& animeName, int frameNo);

			bool play(int animationId, int frameNo);

			const String& playingAnimationName() const;

			const Array<String>& animationNameList() const;

			const Array<String>& partsNameList() const;

			int totalFrame() const;

			int currentFrame() const;

			void setCurrentFrame(int frameNo);

			// 負のフレームや totalFrame()を超えた値をループを考慮したフレーム値に変換する
			int getLoopFrame(int frameNo) const;

			const Array<UserData>& update(double deltaTime);

			void setPartsVisible(int partIndex, bool isVisible);

			bool setFlipPartsVisible(int partIndex);

			bool partsVisible(int partIndex) const;

			void setRotate(const Vec3& rotation);

			void setScale(const Vec3& scale);

			void setFlip(bool isFlip);

			void setMirror(bool isMirror);

			RectF draw(double x, double y, const ColorF& diffuse = Palette::White) const;

			void release();
		};
	}
}
