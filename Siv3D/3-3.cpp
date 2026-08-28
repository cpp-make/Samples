#include <Siv3D.hpp>

void Main()
{
	// ウィンドウのサイズを 1280x720 に変更する
	Window::Resize(1280, 720);

	// 背景色を水色に変更する
	Scene::SetBackground(ColorF{ 0.0, 0.5, 1.0 });

	while (System::Update())
	{

	}
}
