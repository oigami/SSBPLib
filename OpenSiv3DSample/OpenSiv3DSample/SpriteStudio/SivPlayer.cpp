#include<memory>
#include <Siv3D.hpp>
#include <SpriteStudio/Player.hpp>
#include "CPlayer.hpp"

namespace SpriteStudio
{
	void PartInfo::draw() const
	{
		draw(boundsType);
	}

	void PartInfo::draw(BoundsType overrideType) const
	{
		switch ( overrideType )
		{
		default:
		case BoundsType::Invalid:
		case BoundsType::None:
			break;
		case BoundsType::Quad:
			quad.draw(color);
			break;
		case BoundsType::AABB:
			getAABB().draw(color);
			break;
		case BoundsType::Circle:
			getCircle().draw(color);
			break;
		case BoundsType::CircleSMin:
			getCircle().draw(color);
			break;
		case BoundsType::CircleSMax:
			getCircle().draw(color);
			break;
		}
	}

	void PartInfo::drawFrame(int thickness) const
	{
		drawFrame(thickness, boundsType);
	}

	void PartInfo::drawFrame(int thickness, BoundsType overrideType) const
	{
		switch ( overrideType )
		{
		default:
		case BoundsType::Invalid:
		case BoundsType::None:
			break;
		case BoundsType::Quad:
			quad.drawFrame(thickness, color);
			break;
		case BoundsType::AABB:
			getAABB().drawFrame(thickness, color);
			break;
		case BoundsType::Circle:
			getCircle().drawFrame(thickness, color);
			break;
		case BoundsType::CircleSMin:
			getCircle().drawFrame(thickness, color);
			break;
		case BoundsType::CircleSMax:
			getCircle().drawFrame(thickness, color);
			break;
		}
	}

	RectF PartInfo::getAABB() const
	{
		RectF rect;
		rect.x = Min({ quad.p0.x, quad.p1.x, quad.p2.x, quad.p3.x });
		rect.y = Min({ quad.p0.y, quad.p1.y, quad.p2.y, quad.p3.y });
		const auto right = Max({ quad.p0.x, quad.p1.x, quad.p2.x, quad.p3.x });
		const auto bottom = Max({ quad.p0.y, quad.p1.y, quad.p2.y, quad.p3.y });
		rect.w = right - rect.x;
		rect.h = bottom - rect.y;
		return rect;
	}

	Circle SpriteStudio::PartInfo::getCircle() const
	{
		return Circle(center, boundingRadius);
	}




	Player::Player() : m_handle(std::make_shared<detail::CPlayer>())
	{
	}

	Player::Player(const FilePath & filename)
		: m_handle(std::make_shared<detail::CPlayer>(filename))
	{
	}

	Player Player::clone()
	{
		Player player;
		player.m_handle = m_handle->clone();
		return player;
	}

	const Array<String>& Player::animationNameList() const
	{
		return m_handle->animationNameList();
	}

	const Array<String>& Player::partsNameList() const
	{
		return m_handle->partsNameList();
	}

	bool Player::play(int animationIndex, const int frameNo)
	{
		return m_handle->play(animationIndex, frameNo);
	}

	bool Player::play(const String& animationName, const int frameNo)
	{
		return m_handle->play(animationName, frameNo);
	}

	const String& Player::playingAnimationName() const
	{
		return m_handle->playingAnimationName();
	}


	bool Player::partsVisible(int partIndex) const
	{
		return m_handle->partsVisible(partIndex);
	}

	void Player::setPartsVisible(const int partIndex, const bool isVisible)
	{
		m_handle->setPartsVisible(partIndex, isVisible);
	}

	void Player::flipPartsVisible(int partIndex)
	{
		m_handle->setFlipPartsVisible(partIndex);
	}

	int Player::totalFrame() const
	{
		return m_handle->totalFrame();
	}

	int Player::currentFrame() const
	{
		return m_handle->currentFrame();
	}

	void Player::setCurrentFrame(const int frameNo)
	{
		m_handle->setCurrentFrame(frameNo);
	}

	const Array<UserData>& Player::update(const double deltaTime)
	{
		return m_handle->update(deltaTime);
	}

	RectF Player::draw(const ColorF& diffuse) const
	{
		return draw(Window::Center().x, Window::Height(), diffuse);
	}

	RectF Player::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	RectF Player::draw(const double x, const double y, const ColorF& diffuse) const
	{
		return m_handle->draw(x, y, diffuse);
	}

	void Player::release()
	{
		m_handle = std::make_shared<detail::CPlayer>();
	}

	bool Player::isEmpty() const
	{
		return m_handle->isEmpty();
	}

	PlayerX Player::rotate(const Vec3& rotation) const
	{
		return PlayerX(*this, rotation, { 1,1,1 });
	}

	PlayerX Player::rotate(const double x, const  double y, const  double z) const
	{
		return rotate({ x, y, z });
	}

	PlayerX Player::rotate(const double z) const
	{
		return rotate(0, 0, z);
	}

	PlayerX Player::scale(const Vec3& scale) const
	{
		return PlayerX(*this, Vec3::Zero(), scale);
	}

	PlayerX Player::scale(const double xy) const
	{
		return scale(xy, xy, 1);;
	}

	PlayerX Player::scale(const double x, const double y, const double z) const
	{
		return scale({ x, y, z });
	}

	PlayerX Player::flip(const bool doFlip) const
	{
		return PlayerX(*this).flip(doFlip);
	}

	PlayerX Player::mirror(const bool doMirror) const
	{
		return PlayerX(*this).mirror(doMirror);
	}

	PartInfo Player::parts(int partIndex) const
	{
		PartInfo res = m_handle->parts(partIndex);
		const Vec2 pos = { Window::Center().x, Window::Height() };
		res.center += pos;
		for ( auto& i : step(4) )
		{
			res.quad.p(i) += pos;
		}
		return res;
	}

	bool Player::operator==(const Player & player) const
	{
		return m_handle == player.m_handle;
	}


	/**
	 * PlayerX
	 */


	PlayerX& PlayerX::rotate(const Vec3& rotation)
	{
		rotateVec += rotation;
		return *this;
	}


	PlayerX& PlayerX::rotate(const double x, const double y, const double z)
	{
		return rotate({ x, y, z });
	}

	PlayerX& PlayerX::rotate(const double z)
	{
		rotateVec.z += z;
		return *this;
	}


	PlayerX& PlayerX::scale(const Vec3& s)
	{
		// TODO rotateVec に応じて回転させる
		scaleVec *= s;
		return *this;
	}

	PlayerX& PlayerX::scale(const double xy)
	{
		// TODO rotateVec に応じて回転させる
		scaleVec.x *= xy;
		scaleVec.y *= xy;
		return *this;
	}

	PlayerX& PlayerX::scale(const double x, const double y, const double z)
	{
		return scale({ x, y, z });
	}


	PlayerX& PlayerX::flip(const bool doFlip)
	{
		if ( doFlip )
		{
			m_isFlip = !m_isFlip;
		}

		return *this;
	}

	PlayerX& PlayerX::mirror(const bool doMirror)
	{
		if ( doMirror )
		{
			m_isMirror = !m_isMirror;
		}

		return *this;
	}

	PartInfo PlayerX::parts(const Vec3 & pos, int partIndex) const
	{
		auto& handle = m_player.m_handle;
		PartInfo res = handle->parts(partIndex);

		// TODO 三次元回転に対応する
		res.quad = res.quad.rotatedAt(0, 0, rotateVec.z);

		for ( auto& i : step(0) )
		{
			res.quad.p(0) *= scaleVec.xy();
		}
		res.center += pos.xy();
		for ( auto& i : step(4) )
		{
			res.quad.p(i) += pos.xy();
		}
		return res;
	}

	PartInfo PlayerX::parts(int partIndex) const
	{
		return parts({ Window::Center().x, Window::Height(),0 }, partIndex);
	}

	RectF PlayerX::draw(const ColorF& diffuse) const
	{
		return draw(Window::Center().x, Window::Height(), diffuse);
	}

	RectF PlayerX::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	RectF PlayerX::draw(const double x, const double y, const ColorF& diffuse) const
	{
		auto& handle = m_player.m_handle;
		handle->setRotate(rotateVec);
		handle->setScale(scaleVec);
		handle->setFlip(m_isFlip);
		handle->setMirror(m_isMirror);

		const auto res = handle->draw(x, y, diffuse);

		// 描画後に状態を戻しておく
		handle->setRotate({ 0, 0, 0 });
		handle->setScale({ 1, 1, 1 });
		handle->setFlip(false);
		handle->setMirror(false);

		return res;
	}
}
