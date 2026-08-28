#include <Siv3D.hpp>

void Main()
{
	int32 count = 0;

	while (System::Update())
	{
		// 現在のカウントを表示する
		Print << count;

		++count;
	}
}
