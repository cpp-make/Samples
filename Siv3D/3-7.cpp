#include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	const Texture ball{ U"⚽"_emoji };

	// サッカーボールの初期位置
	Vec2 pos{ 400, 300 };

	while (System::Update())
	{
		// 前フレームからの経過時間（秒）* 200
		double move = (Scene::DeltaTime() * 200);

		if (KeyRight.pressed())
		{
			pos.x += move;
		}

		if (KeyLeft.pressed())
		{
			pos.x -= move;
		}

		if (KeyUp.pressed())
		{
			pos.y -= move;
		}

		if (KeyDown.pressed())
		{
			pos.y += move;
		}

		ball.drawAt(pos);
	}
}
