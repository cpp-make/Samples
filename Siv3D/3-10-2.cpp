#include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	Font font{ FontMethod::MSDF, 40, Typeface::Bold };

	Circle largeCircle{ 400, 200, 100 };

	while (System::Update())
	{
		Circle smallCircle{ Cursor::Pos(), 20 };

		largeCircle.draw();

		smallCircle.draw(ColorF{ 0.6 });

		// もし 2 つの Circle が交差していたら
		if (smallCircle.intersects(largeCircle))
		{
			font(U"交差").draw(30, Vec2{ 40, 40 }, ColorF{ 0.1 });
		}
	}
}
