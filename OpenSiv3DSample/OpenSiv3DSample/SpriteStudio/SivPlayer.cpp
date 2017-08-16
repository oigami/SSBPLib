#include<memory>
#include <Siv3D.hpp>
#include <SpriteStudio/Player.hpp>
#include "CPlayer.hpp"

namespace SpriteStudio
{

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

	bool Player::play(int animationIndex)
	{
		return m_handle->play(animationIndex);
	}

	bool Player::play(const String& animationName)
	{
		return m_handle->play(animationName);
	}


	void Player::setVisible(int partIndex, bool isVisible)
	{
		m_handle->setVisible(partIndex, isVisible);
	}

	const Array<UserData>& Player::update(double deltaTime)
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

	PlayerX Player::rotate(double x, double y, double z) const
	{
		return rotate({ x, y, z });
	}

	PlayerX Player::rotate(double z) const
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

	PlayerX Player::flip(bool doFlip) const
	{
		return PlayerX(*this).flip(doFlip);
	}

	PlayerX Player::mirror(bool doMirror) const
	{
		return PlayerX(*this).mirror(doMirror);
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


	PlayerX& PlayerX::flip(bool doFlip)
	{
		if ( doFlip )
		{
			m_isFlip = !m_isFlip;
		}

		return *this;
	}

	PlayerX& PlayerX::mirror(bool doMirror)
	{
		if ( doMirror )
		{
			m_isMirror = !m_isMirror;
		}

		return *this;
	}

	RectF PlayerX::draw(const ColorF& diffuse) const
	{
		return draw(Window::Center().x, Window::Height(), diffuse);
	}

	RectF PlayerX::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	RectF PlayerX::draw(double x, double y, const ColorF& diffuse) const
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
