﻿#include <Siv3D.hpp>
#include "SpriteStudio/Player.hpp"

void MultiDrawAndColorTest(SpriteStudio::Player player)
{

	SecondsF time{};
	Seconds nowCount{};

	Font font(24);

	while ( System::Update() && !MouseM.down() )
	{
		time += SecondsF(System::DeltaTime());
		player.update();

		ColorF color = Palette::Red;
		color.a = 0.5;
		player.draw(Window::Width() / 3, Window::Height(), color);

		player.draw(Window::Width() * 2 / 3, Window::Height());

		player.draw(Cursor::Pos());

		auto nextCount = DurationCast<Seconds>(time) / 3;
		if ( nowCount < nextCount && nextCount.count() < static_cast<long long>(player.animationNameList().size()) )
		{
			nowCount = nextCount;
			player.play(static_cast<int>(nowCount.count()));
		}

		font(player.animationNameList()[nowCount.count()]).draw();

		Circle(Cursor::Pos(), 40).draw(Palette::Red);
	}
}

void RotateAndScaleTest(SpriteStudio::Player player)
{

	double time = 0;

	player.play(0);

	while ( System::Update() && !MouseM.down() )
	{
		player.update();
		time += System::DeltaTime();

		player.rotate((double)Cursor::Pos().x / Window::Width() * Math::TwoPi)
			.scale(0.5)
			.draw(Window::Width() / 3);

		player.rotate(45_deg).rotate(30_deg, 0, 0).scale(sin(time)).draw(Window::Width() * 2 / 3);
	}
}

void FlipMirrorTest(SpriteStudio::Player player)
{
	player.play(0);

	while ( System::Update() && !MouseM.down() )
	{
		player.mirror(MouseL.pressed()).flip(MouseR.pressed()).draw();
	}
}


// TODO テスト用データの作成をする
void UserDataTest(SpriteStudio::Player player)
{
	player.play(0);

	while ( System::Update() && !MouseM.down() )
	{
		auto userData = player.update();
		for ( auto& i : userData )
		{
			Print << i.integer << i.rect << i.point << i.str;
		}
	}
}

void VisiblePartTest(SpriteStudio::Player player)
{
	Print << player.partsNameList().size();

	for ( auto& i : player.partsNameList() )
	{
		Print << i;
	}

	// 境界チェック
	for ( auto& i : step(-100, 100) )
	{
		player.setPartsVisible(i, false);
	}
	for ( auto& i : step(-100, 100) )
	{
		player.setPartsVisible(i, true);
	}

	player.play(0);

	Font font(30);

	while ( System::Update() && !MouseM.down() )
	{
		auto partsNameList = player.partsNameList();
		for ( auto& i : step(static_cast<int>(partsNameList.size())) )
		{
			const RectF rect = font(partsNameList[i]).draw(0, i * 40);
			if ( rect.intersects(Cursor::Pos()) )
			{
				rect.drawFrame(5, Palette::Red);
				if ( MouseL.down() )
				{
					player.flipPartsVisible(i);
				}
			}
		}

		player.update();
		player.draw();
	}
}

void DrawRectTest(SpriteStudio::Player player)
{
	player.play(3);

	Font font(40);
	while ( System::Update() && !MouseM.down() )
	{
		player.update();

		RectF rect = player.scale(0.3).draw(Window::Width() * 2 / 3, Window::Height() - 30);
		rect.drawFrame(5);
		font(rect).draw();

		player.scale(0.3).draw(Window::Width() / 3, Window::Height() - 30).drawFrame(5);
	}
}

void CloneTest(SpriteStudio::Player player)
{
	player.play(0);

	auto clonePlayer = player.clone();
	clonePlayer.play(0);


	while ( System::Update() && !MouseM.down() )
	{
		player.update();

		if ( MouseL.pressed() )
		{
			clonePlayer.update();
		}
		player.draw(Window::Width() / 3);

		clonePlayer.draw(Window::Width() * 2 / 3).drawFrame(2);
	}
}

void EmptyTest(SpriteStudio::Player player)
{
	SpriteStudio::Player emptyPlayer;
	emptyPlayer.play(0);

	auto clonePlayer = emptyPlayer.clone();
	clonePlayer.play(0);


	while ( System::Update() && !MouseM.down() )
	{
		emptyPlayer.update();

		if ( MouseL.pressed() )
		{
			clonePlayer.update();
		}
		emptyPlayer.draw(Window::Width() / 3);

		clonePlayer.draw(Window::Width() * 2 / 3);
	}


}

void InvalidFileTest(SpriteStudio::Player player)
{
	SpriteStudio::Player notExistFilePlayer(L"not_exsit_file_path");
	notExistFilePlayer.play(0);

	SpriteStudio::Player invalidFilePlayer(L"character_template_comipo/character_2head.png");
	invalidFilePlayer.play(0);

	while ( System::Update() && !MouseM.down() )
	{
		notExistFilePlayer.update();

		if ( MouseL.pressed() )
		{
			invalidFilePlayer.update();
		}
		notExistFilePlayer.draw(Window::Width() / 3);
		notExistFilePlayer.rotate(0).rotate(0, 0, 0).rotate(Vec3{ 0,0,0 }).scale(1).scale(1, 1, 1).scale({ 1,1,1 }).flip().mirror().draw();

		invalidFilePlayer.draw(Window::Width() * 2 / 3);
	}
}

void CurrentFrameTest(SpriteStudio::Player player)
{
	Font font(40);

	player.play(0);

	for ( auto& i : step(1000) )
	{
		player.play(0, i);
	}

	while ( System::Update() && !MouseM.down() )
	{
		if ( MouseL.pressed() )
		{
			player.setCurrentFrame(Cursor::Pos().x / 10);
		}
		else
		{
			player.update();
		}

		font(player.currentFrame(), L"  ", player.totalFrame()).draw();

		player.draw();
	}
}

void BoundsCheckTest(SpriteStudio::Player player)
{

	double time = 0;

	while ( System::Update() && !MouseM.down() )
	{
		//time += System::DeltaTime();
		if ( MouseL.pressed() )
		{
			player.update();
		}

		for ( auto& i : step(player.partsNameList().size()) )
		{
			//player.parts(i).drawFrame(SpriteStudio::PartInfo::BoundsType::Quad, 2);

			player.rotate(time).parts(i).drawFrame(2, SpriteStudio::BoundsType::Quad);
		}

		player.rotate(time).draw();

		if ( player.rotate(time).parts(9).intersects(Circle(Cursor::Pos(), 10), SpriteStudio::BoundsType::Quad) )
		{
			Circle(Cursor::Pos(), 10).draw(Palette::Red);
		}
		else
		{
			Circle(Cursor::Pos(), 10).draw(Palette::White);
		}

	}
}


void Main()
{
	SpriteStudio::Player player(L"character_template_comipo/character_template1.ssbp");
	for ( auto& i : player.animationNameList() )
	{
		Print << i;
	}

	//Texture tex(L"character_template_comipo/character_2head.png");
	//double time = 0;

	//while ( System::Update() )
	//{
	//	time += System::DeltaTime();
	//	tex.scale(0.2).flip().draw();
	//}

	Font font(30);
	Array<std::pair<String, std::function<void(SpriteStudio::Player)>>> func;
	func.push_back({ L"FlipMirrorTest", FlipMirrorTest });
	func.push_back({ L"RotateAndScaleTest", RotateAndScaleTest });
	func.push_back({ L"MultiDrawAndColorTest", MultiDrawAndColorTest });
	func.push_back({ L"UserDataTest", UserDataTest });
	func.push_back({ L"VisiblePartTest", VisiblePartTest });
	func.push_back({ L"DrawRectTest", DrawRectTest });
	func.push_back({ L"CloneTest", CloneTest });
	func.push_back({ L"EmptyTest", EmptyTest });
	func.push_back({ L"InvalidFileTest", InvalidFileTest });
	func.push_back({ L"CurrentFrameTest", CurrentFrameTest });
	func.push_back({ L"BoundsCheckTest", BoundsCheckTest });

	while ( System::Update() )
	{
		for ( auto& i : step(static_cast<int>(func.size())) )
		{
			const RectF rect = font(func[i].first).draw(0, i * 40);
			if ( rect.intersects(Cursor::Pos()) )
			{
				rect.drawFrame(5, Palette::Red);
				if ( MouseL.down() )
				{
					func[i].second(player);
				}
			}
		}
	}
}