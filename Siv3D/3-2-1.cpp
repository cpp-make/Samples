#include <Siv3D.hpp>

void Main()
{
	Print << (100 + 23);

	Print << U"Hello";

	// 文字列クラス
	String s = U"Siv3D";
	Print << s;

	// int32 型の配列
	Array<int32> a = { 1, 2, 3, 4, 5 };
	Print << a;

	// 座標（整数）を表す Point 型
	Print << Point{ 10, 20 };

	// 座標（浮動小数点数）を表す Vec2 型
	Print << Vec2{ 1.5, 2.5 };

	// 色を表す ColorF 型
	Print << ColorF{ 1.0, 0.5, 0.0, 1.0 };

	while (System::Update())
	{

	}
}
