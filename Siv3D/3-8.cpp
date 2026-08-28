#include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// フォントファイルからフォントを作成する
	const Font font1{ FontMethod::MSDF, 48,
		U"example/font/RocknRoll/RocknRollOne-Regular.ttf" };

	// 同梱のフォントファイル（Bold）からフォントを作成する
	const Font font2{ FontMethod::MSDF, 48, Typeface::Bold };

	while (System::Update())
	{
		font1(U"Hello, Siv3D!")
			.draw(50, Vec2{ 100, 100 }, ColorF{ 0.2 });

		font2(U"Hello, C++!")
			.draw(80, Vec2{ 100, 200 }, ColorF{ 0.0, 0.5, 1.0 });
	}
}
