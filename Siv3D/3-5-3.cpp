#include <Siv3D.hpp>

void Main()
{
	while (System::Update())
	{
		if (MouseL.down())
		{
			Print << U"左ボタンがクリックされた";
		}

		// マウスの左ボタンが押されている間
		if (MouseL.pressed())
		{
			// 円を描く
			Circle{ Cursor::Pos(), 50 }.draw();
		}

		if (MouseL.up())
		{
			Print << U"左ボタンが離された";
		}
	}
}
