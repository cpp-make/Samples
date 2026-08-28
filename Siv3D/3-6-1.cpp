#include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	// 画像ファイルからテクスチャを作成する
	const Texture t1{ U"example/windmill.png" };

	// 絵文字からテクスチャを作成する
	const Texture t2{ U"🍎"_emoji };

	const Texture t3{ U"🐈"_emoji };

	while (System::Update())
	{
		// (20, 20) を左上にして描画する
		t1.draw(20, 20);

		// (300, 500) を中心にして描画する
		t2.drawAt(300, 500);

		// カーソルの位置を中心にして描画する
		t3.drawAt(Cursor::Pos());
	}
}
