#include <Siv3D.hpp>

void Main()
{
	while (System::Update())
	{
		// マウスカーソルの座標を中心とする半径 50 の円を描画する
		Circle{ Cursor::Pos(), 50 }.draw();
	}
}
