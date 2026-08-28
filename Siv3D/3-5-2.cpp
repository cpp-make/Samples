#include <Siv3D.hpp>

void Main()
{
	while (System::Update())
	{
		// マウスの左ボタンがクリックされたら
		if (MouseL.down())
		{
			// 円を描く
			Circle{ Cursor::Pos(), 50 }.draw();
		}
	}
}
