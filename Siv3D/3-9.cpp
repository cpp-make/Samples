#include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// 音声ファイルから作成する
	const Audio bgm{ U"example/test.mp3" };

	// 同梱のサウンドフォントから音声を作成する
	// マリンバの音、高さは C4, 鳴らす時間は 0.5 秒
	const Audio sound{ Wave{ GMInstrument::Marimba,
		PianoKey::C4, 0.5s } };

	while (System::Update())
	{
		if (KeySpace.down())
		{
			// BGM を再生する
			bgm.play();
		}

		if (KeyEnter.down())
		{
			// 効果音を再生する
			sound.playOneShot();
		}
	}
}
