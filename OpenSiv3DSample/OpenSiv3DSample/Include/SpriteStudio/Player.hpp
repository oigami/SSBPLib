﻿#pragma once
#include <Siv3D.hpp>

namespace SpriteStudio
{
	namespace detail
	{
		class CPlayer;
	}

	struct UserData
	{
		int partIndex;	/// パーツインデックス
		int frameNo;

		Optional<int> integer;	/// 整数データ
		Optional<Rect> rect;	/// 矩形データ[Left, Top, Right, Bottom]
		Optional<Point> point;	/// 座標データ[X, Y]
		Optional<String> str;	/// 文字列データ 
	};

	enum class BoundsType
	{
		Invalid = -1,
		None,			///< 当たり判定として使わない。
		Quad,			///< 自在に変形する四辺形。頂点変形など適用後の４角を結んだ領域。最も重い。
		AABB,			///< 回転しない全体を囲む矩形で交差判定
		Circle,			///< 真円の半径で距離により判定する
		CircleSMin,		///< 真円の半径で距離により判定する (スケールはx,yの最小値をとる）
		CircleSMax,		///< 真円の半径で距離により判定する (スケールはx,yの最大値をとる）
	};

	struct PartInfo
	{


		Vec2 center;	/// パーツのワールド行列
		Quad quad;

		BoundsType boundsType;		/// 当たり判定種類
		float boundingRadius;			/// SS5アトリビュート：当たり半径
		ColorF color;


	public:

		template<class Shape2DType>
		bool contains(const Shape2DType& shapes, BoundsType type) const
		{
			switch ( type )
			{
			default:
			case BoundsType::Invalid:
			case BoundsType::None:		return false;
			case BoundsType::Quad:		return quad.contains(shapes);
			case BoundsType::AABB:		return getAABB().contains(shapes);
			case BoundsType::Circle:		return getCircle().contains(shapes);
			case BoundsType::CircleSMin:	return getCircle().contains(shapes);
			case BoundsType::CircleSMax:	return getCircle().contains(shapes);
			}
		}

		template<class Shape2DType>
		bool contains(const Shape2DType& shapes) const
		{
			return contains(shapes, boundsType);
		}

		template<class Shape2DType>
		bool intersects(const Shape2DType& shapes, const BoundsType type) const
		{
			switch ( type )
			{
			default:
			case BoundsType::Invalid:
			case BoundsType::None:		return false;
			case BoundsType::Quad:		return quad.intersects(shapes);
			case BoundsType::AABB:		return getAABB().intersects(shapes);
			case BoundsType::Circle:		return getCircle().intersects(shapes);
			case BoundsType::CircleSMin:	return getCircle().intersects(shapes);
			case BoundsType::CircleSMax:	return getCircle().intersects(shapes);
			}
		}

		template<class Shape2DType>
		bool intersects(const Shape2DType& shapes) const
		{
			return intersects(shapes, boundsType);
		}

		void draw() const;

		void draw(BoundsType overrideType) const;

		void drawFrame(int thickness) const;

		void drawFrame(int thickness, BoundsType overrideType) const;

	private:

		RectF getAABB() const;

		Circle getCircle() const;
	};

	class PlayerX;

	class Player
	{
	private:

		friend class PlayerX;

		std::shared_ptr<detail::CPlayer> m_handle;

	public:

		Player();

		explicit Player(const FilePath& filename);

		Player clone();

		bool play(int animationIndex, int frameNo = 0);

		bool play(const String& animationName, int frameNo = 0);


		bool partsVisible(int partIndex) const;

		void setPartsVisible(int partIndex, bool isVisible);

		void flipPartsVisible(int partIndex);


		const Array<String>& animationNameList() const;

		const Array<String>& partsNameList() const;

		const String& playingAnimationName() const;

		int totalFrame() const;

		int currentFrame() const;

		void setCurrentFrame(int frameNo);

		const Array<UserData>& update(double deltaTime = System::DeltaTime());


		RectF draw(const ColorF& diffuse = Palette::White) const;

		RectF draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		RectF draw(double x, double y = Window::Height(), const ColorF& diffuse = Palette::White) const;



		PlayerX rotate(double z) const;

		PlayerX rotate(const Vec3& rotation) const;

		PlayerX rotate(double x, double y, double z) const;


		PlayerX scale(double xy) const;

		PlayerX scale(const Vec3& s) const;

		PlayerX scale(double x, double y, double z = 1.0) const;

		PlayerX flip(bool doFlip = true) const;

		PlayerX mirror(bool doMirror = true) const;


		PartInfo parts(int partIndex) const;


		void release();

		bool isEmpty() const;

		explicit operator bool() const
		{
			return !isEmpty();
		}

		bool operator ==(const Player& player) const;

		bool operator !=(const Player& player) const { return !(*this == player); }

	};

	class PlayerX
	{
	private:

		Player m_player;

		bool m_isFlip;

		bool m_isMirror;

	public:

		Vec3 rotateVec;

		Vec3 scaleVec;

		explicit PlayerX(Player player, const Vec3& rotate = Vec3::Zero(), const Vec3& scale = Vec3::One())
			: m_player(std::move(player)), m_isFlip(false), m_isMirror(false), rotateVec(rotate), scaleVec(scale)
		{
		}

		RectF draw(const ColorF& diffuse = Palette::White) const;

		RectF draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		RectF draw(double x, double y = Window::Height(), const ColorF& diffuse = Palette::White) const;


		PlayerX& rotate(const Vec3& rotation);

		PlayerX& rotate(double x, double y, double z);

		PlayerX& rotate(double z);

		PlayerX& scale(double xy);

		PlayerX& scale(const Vec3& s);

		PlayerX& scale(double x, double y, double z = 1.0);

		PlayerX& flip(bool doFlip = true);

		PlayerX& mirror(bool doMirror = true);

		PartInfo parts(const Vec3& pos, int partIndex) const;

		PartInfo parts(int partIndex) const;

	};
}
