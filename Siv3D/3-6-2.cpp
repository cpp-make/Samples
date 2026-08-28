#include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	const Texture t1{ U"example/windmill.png" };
	const Texture t2{ U"🍎"_emoji };
	const Texture t3{ U"🐈"_emoji };

	while (System::Update())
	{
		// 50% の大きさで描画する
		t1.scaled(0.5).draw(20, 20);

		// 時計回りに 45° 回転して描画する
		t2.rotated(45_deg).drawAt(300, 400);

		// 2 倍の大きさで、反時計回りに 15° 回転して描画する
		t3.scaled(2.0).rotated(-15_deg)
			.drawAt(Cursor::Pos());
	}
}
