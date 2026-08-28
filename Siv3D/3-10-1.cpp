#include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	Circle circle{ 200, 200, 50 };

	Rect rect{ 400, 400, 200, 40 };

	while (System::Update())
	{
		ClearPrint();

		// 円の上にマウスカーソルがあれば
		if (circle.mouseOver())
		{
			Print << U"円の上にある";
		}

		// 長方形の上にマウスカーソルがあれば
		if (rect.mouseOver())
		{
			Print << U"長方形の上にある";
		}

		circle.draw();

		rect.draw();
	}
}
